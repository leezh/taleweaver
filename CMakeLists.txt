cmake_minimum_required(VERSION 3.11)
project(taleweaver C)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE "extlib/linmath")

add_subdirectory("extlib/glad/cmake" glad_cmake)
glad_add_library(glad REPRODUCIBLE API gl:core=4.3)
target_link_libraries(${PROJECT_NAME} glad)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory("extlib/sdl")
target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)

target_sources(${PROJECT_NAME} PRIVATE
  src/main.h
  src/main.c
  src/render.h
  src/render.c
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/resource")
include(resource)
add_resource(res
  CSTR
    shaders/render_flat.vert
    shaders/render_flat.frag
)
target_link_libraries(${PROJECT_NAME} res)
