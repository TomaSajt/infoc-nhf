cmake_minimum_required(VERSION 3.5)
project(infoc-nhf)

# clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(
    SDL3 REQUIRED CONFIG REQUIRED
    COMPONENTS SDL3-shared
)

add_subdirectory(third_party/SDL3_gfx)

add_compile_options(
    -Wall
    -Werror
)

add_executable(infoc-nhf WIN32
    main.c
)

target_include_directories(infoc-nhf PRIVATE
    third_party/SDL3_gfx
)

target_link_libraries(infoc-nhf PRIVATE
    SDL3::SDL3
    SDL3_gfx_Static
)
