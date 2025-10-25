cmake_minimum_required(VERSION 3.11)
project(infoc-nhf)

# clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(
    SDL3 REQUIRED CONFIG REQUIRED
    COMPONENTS SDL3-shared
)

include(FetchContent)

# fork of SDL2_gfx for SDL3 support
FetchContent_Declare(
  SDL3_gfx
  GIT_REPOSITORY https://github.com/sabdul-khabir/SDL3_gfx.git
  GIT_TAG        add4f8c517d7b0275a6dcd67638d073be051ea88
)

FetchContent_MakeAvailable(SDL3_gfx)

add_compile_options(
    -Wall
    -Werror
)
add_executable(infoc-nhf WIN32
    main.c
    geom.c
    geom_defs.c
    savedata.c
)

target_link_libraries(infoc-nhf PRIVATE
    m # Maths
    SDL3::SDL3
    SDL3_gfx_Static
)
