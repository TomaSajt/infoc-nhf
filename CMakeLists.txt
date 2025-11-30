cmake_minimum_required(VERSION 3.11)
project(infoc-nhf)

# clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(
    SDL3 REQUIRED CONFIG REQUIRED
    COMPONENTS SDL3-shared
)

find_package(SDL3_ttf REQUIRED CONFIG REQUIRED)

include(FetchContent)

# fork of SDL2_gfx for SDL3 support
# TODO: maybe vendor used functions, or maybe just don't use them
FetchContent_Declare(
  SDL3_gfx
  GIT_REPOSITORY https://github.com/sabdul-khabir/SDL3_gfx.git
  GIT_TAG        27b177ba86f13ea3c106c26fcd48169f482a10e1
)

FetchContent_MakeAvailable(SDL3_gfx)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -include ${CMAKE_SOURCE_DIR}/debugmalloc.h
)

add_executable(infoc-nhf WIN32
    draw.c
    hover.c
    main.c

    save/data.c
    save/dialog.c

    geom/defs.c
    geom/state.c
    geom/util.c

    mode/defs.c
    mode/util.c
    mode/circle.c
    mode/circle_by_len.c
    mode/delete.c
    mode/linelike.c
    mode/midpoint.c
    mode/move.c
    mode/par_per.c
    mode/point.c
)

target_link_libraries(infoc-nhf PRIVATE
    m # Maths
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_gfx_Static
)

include(GNUInstallDirs)

install(TARGETS infoc-nhf
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
