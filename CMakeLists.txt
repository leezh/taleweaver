cmake_minimum_required(VERSION 3.11)
project(taleweaver C)

set(
  PROJECT_SOURCES
  src/main.c
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "extlib/linmath")

add_subdirectory("extlib/glad/cmake" glad_cmake)
glad_add_library(glad REPRODUCIBLE API gl:core=4.3)
target_link_libraries(${PROJECT_NAME} glad)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory("extlib/sdl")
target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)
