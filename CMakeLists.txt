cmake_minimum_required(VERSION 3.11)
project(taleweaver C)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE "extlib/linmath")
target_include_directories(${PROJECT_NAME} PRIVATE "extlib/stb")

add_subdirectory("extlib/glad/cmake" glad_cmake)
glad_add_library(glad REPRODUCIBLE API gl:core=3.3)
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
  src/shader.h
  src/shader.c
  src/textures.h
  src/textures.c
  src/heightmap.h
  src/heightmap.c
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/resource")
include(resource)
add_resource(res
  CSTR
    shaders/render.vert
    shaders/render.frag
    shaders/heightmap.vert
    shaders/heightmap.frag
)
target_link_libraries(${PROJECT_NAME} res)
