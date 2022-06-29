cmake_minimum_required(VERSION 3.11)
project(taleweaver C CXX)

add_executable(${PROJECT_NAME})

# == Dependencies ==
find_package(Git REQUIRED)
execute_process(COMMAND
  ${GIT_EXECUTABLE} submodule update --init --recursive
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_subdirectory("extlib/entt" entt EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} EnTT::EnTT)

add_subdirectory("extlib/glm" glm EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} glm::glm)

target_include_directories(${PROJECT_NAME} PRIVATE "extlib/stb")

add_subdirectory("extlib/glad/cmake" glad_cmake EXCLUDE_FROM_ALL)
glad_add_library(glad REPRODUCIBLE API gl:core=3.3)
target_link_libraries(${PROJECT_NAME} glad)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory("extlib/sdl" sdl EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)

# == Source Files ==
target_sources(${PROJECT_NAME} PRIVATE
  LICENSE.md
  README.md
  images/heightmap.png
  shaders/render.vert
  shaders/render.frag
  shaders/heightmap.vert
  shaders/heightmap.frag
  src/spatial.hpp
  src/camera.hpp
  src/window.hpp
  src/window.cpp
  src/main.cpp
  src/render.hpp
  src/render.cpp
  src/shader.hpp
  src/shader.cpp
  src/textures.hpp
  src/textures.cpp
  src/heightmap.hpp
  src/heightmap.cpp
)

# == Compiled-in Resources ==
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/resource")
include(resource)
add_resource(res
  CSTR
    shaders/render.vert
    shaders/render.frag
    shaders/heightmap.vert
    shaders/heightmap.frag
  HEX
    images/heightmap.png
)
target_link_libraries(${PROJECT_NAME} res)

# == Install Targets ==
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY images DESTINATION share/taleweaver
  FILES_MATCHING
  PATTERN "*.png"
)
