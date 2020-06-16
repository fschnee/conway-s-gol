project('conway', 'cpp',
    version: '0.0.0',
    default_options : [
        'cpp_std=c++17',
        'buildtype=release',
    ],
)

sources = files(
    'src/main.cpp',
)

deps = [
    dependency('OpenMP'),
    dependency('sdl2', fallback: ['sdl2', 'sdl2_dep']),
]

# from: https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
link_args = [
    '-Wall', # reasonable and standard
    '-Wextra', # reasonable and standard
    '-Wshadow', # warn the user if a variable declaration shadows one from a parent context
    '-Wnon-virtual-dtor', # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    '-Wold-style-cast', # warn for c-style casts
    '-Wcast-align', # warn for potential performance problem casts
    '-Wunused', # warn on anything being unused
    '-Woverloaded-virtual', # warn if you overload (not override) a virtual function
    '-Wpedantic', # (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
    '-Wconversion', # warn on type conversions that may lose data
    '-Wsign-conversion', # (Clang all versions, GCC >= 4.3) warn on sign conversions
    '-Wmisleading-indentation', # (only in GCC >= 6.0) warn if indentation implies blocks where blocks do not exist
    '-Wduplicated-cond', # (only in GCC >= 6.0) warn if if / else chain has duplicated conditions
    '-Wduplicated-branches', # (only in GCC >= 7.0) warn if if / else branches have duplicated code
    '-Wlogical-op', # (only in GCC) warn about logical operations being used where bitwise were probably wanted
    '-Wnull-dereference', # (only in GCC >= 6.0) warn if a null dereference is detected
    '-Wuseless-cast', # (only in GCC >= 4.8) warn if you perform a cast to the same type
    '-Wdouble-promotion', # (GCC >= 4.6, Clang >= 3.8) warn if float is implicit promoted to double
    '-Wformat=2', # warn on security issues around functions that format output (ie printf)
]

conway_exe = executable(
    'conway',
    sources,
    dependencies: deps,
    link_args: link_args,
)
