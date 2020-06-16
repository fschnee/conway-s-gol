#include <SDL.h>

#include "mappings.hpp"
#include "type_aliases.hpp"

/* cool configs */
//---- Default conway's game of life
const auto cell_lives = u8{ 1 };
//---- "My leg is numb"
//#define ACCUMULATE_ON 5
// const auto cell_lives = u8{10};
//---- Ants walking on tunnels
//#define ACCUMULATE_ON 2
// const auto cell_lives = u8{3};

#include <random>
auto random_populate(u8* out, u64 size, u8 lives)
{
    auto dist   = std::uniform_int_distribution<u8>{ 0, lives };
    auto engine = std::default_random_engine();
    for (u64 x = 0; x < size; ++x) {
        out[x] = dist(engine) * 255 / lives;
    }
}

#include <algorithm>
#include <array>
#include <functional>
#include <omp.h>
auto conway(const u8* in, u8* out, u32 width, u32 height, u8 lives)
{
    using std::make_pair;
    using pref = std::reference_wrapper<const std::pair<i8, i8>>;

    const auto cell_life_value = 255 / lives;

    static const constexpr auto get_valid_offsets =
        [](std::pair<u32, u32> pixel, u32 width, u32 height) -> std::array<std::pair<bool, pref>, 8> {
        // clang-format off
        static const constexpr std::pair<i8, i8> neighbour_offsets[8] = { 
            make_pair(-1, -1), make_pair(0, -1), make_pair(1, -1),
            make_pair(-1,  0), /*pixel here,*/   make_pair(1,  0),
            make_pair(-1,  1), make_pair(0,  1), make_pair(1,  1)
        };
        // clang-format on

        const auto [x, y] = pixel;
        return {
            make_pair((x > 0 && y > 0), std::cref(neighbour_offsets[0])),
            make_pair((y > 0), std::cref(neighbour_offsets[1])),
            make_pair((x < width - 1 && y > 0), std::cref(neighbour_offsets[2])),

            make_pair((x > 0), std::cref(neighbour_offsets[3])),
            /* pixel here, */
            make_pair((x < width - 1), std::cref(neighbour_offsets[4])),

            make_pair((x > 0 && y < height - 1), std::cref(neighbour_offsets[5])),
            make_pair((y < height - 1), std::cref(neighbour_offsets[6])),
            make_pair((x < width - 1 && y < height - 1), std::cref(neighbour_offsets[7])),
        };
    };

#pragma omp parallel for
    for (u64 i = 0; i < width * height; ++i) {
        const auto pixel   = map_1d_to_2d(i, width);
        const auto offsets = get_valid_offsets(pixel, width, height);

        const auto [x, y]            = pixel;
        const auto living_neighbours = std::transform_reduce(
            offsets.begin(), offsets.end(), 0, std::plus<u8>(), [in, width, x, y](auto p) -> u8 {
                auto [offset_valid, offset] = p;
                [[unlikely]] if (!offset_valid) { return 0; }

                auto [off_x, off_y] = offset.get();
                return in[map_2d_to_1d(x + off_x, y + off_y, width)] != 0;
            });
        const auto cell_status = in[i];

        // Any live cell with fewer than two live neighbours dies, as if by
        // underpopulation. Any live cell with two or three live neighbours
        // lives on to the next generation. Any live cell with more than three
        // live neighbours dies, as if by overpopulation. Any dead cell with
        // exactly three live neighbours becomes a live cell, as if by
        // reproduction.
        if (cell_status) {
#ifdef ACCUMULATE_ON
            if (living_neighbours == ACCUMULATE_ON) {
                out[i] = std::clamp<u16>(static_cast<u16>(cell_status) + cell_life_value, 0, 255);
            } else
#endif
                if (living_neighbours < 2 || living_neighbours > 3) {
                out[i] = std::clamp<i16>(static_cast<i16>(cell_status) - cell_life_value, 0, 255);
            } else {
                out[i] = cell_status;
            }
        } else if (living_neighbours == 3) {
            out[i] = cell_life_value;
        } else {
            out[i] = 0;
        }
    }
}

#include <iostream>
#include <memory>    // smart ptrs
#include <stdexcept> // std::runtime_error
int main()
{
    auto sdl_context = [] {
        if (auto status = SDL_Init(SDL_INIT_VIDEO); status == 0) {
            // I'll admit, it's a bit dumb (a unique_pointer just to run a
            // function on deletion) but it gets the job done cleanly so shut
            // up. A scope_exit RAII like thing would be more appropriate but we
            // don't have those yet.
            auto cleanup = []([[maybe_unused]] void*) { SDL_Quit(); };
            return std::unique_ptr<void, decltype(cleanup)>(0, std::move(cleanup));
        } else {
            std::cout << "Error initializing SDL2 (code = " << status << "): " << SDL_GetError();
            throw std::runtime_error{ "SDL2 failed to initialize" };
        }
    }();

    const auto winwidth  = 1920;
    const auto winheight = 1080;
    auto window          = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>{
        SDL_CreateWindow("Howdy, world",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         winwidth,
                         winheight,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN),
        [](auto window) {
            if (window) SDL_DestroyWindow(window);
        }
    };
    if (!window) {
        std::cout << "Error creating window: " << SDL_GetError();
        return 1;
    }

    auto screen                = SDL_GetWindowSurface(window.get());
    const auto bytes_per_pixel = screen->format->BytesPerPixel;

    auto last_frame = std::vector<u8>(winwidth * winheight);
    random_populate(last_frame.data(), last_frame.size(), cell_lives);
    auto this_frame = std::vector<u8>(winwidth * winheight);

    const auto fps_target       = 60;
    const auto frametime_target = static_cast<u32>(1.0f / f32{ fps_target } * 1000);
    for (bool quit = false; !quit;) {
        auto start = SDL_GetTicks();

        for (SDL_Event e; SDL_PollEvent(&e);) {
            if (e.type == SDL_QUIT) {
                quit = true;
            };
        }

        conway(last_frame.data(), this_frame.data(), winwidth, winheight, cell_lives);

        if (SDL_MUSTLOCK(screen)) {
            while (!SDL_LockSurface(screen)) {
            }
        }

#pragma omp parallel for
        for (auto i = 0; i < screen->w * screen->h * bytes_per_pixel; i += bytes_per_pixel) {
            for (auto offset = 0; offset < bytes_per_pixel; offset++) {
                *((u8*)(screen->pixels) + i + offset) = this_frame[i / bytes_per_pixel];
            }
        }

        if (SDL_MUSTLOCK(screen)) {
            SDL_UnlockSurface(screen);
        }

        std::swap(this_frame, last_frame);

        SDL_UpdateWindowSurface(window.get());

        auto end       = SDL_GetTicks();
        auto frametime = end - start;
        if (frametime < frametime_target) {
            SDL_Delay(frametime_target - frametime);
        }
    }

    return 0;
}
