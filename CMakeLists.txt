cmake_minimum_required(VERSION 3.18)
project(taleweaver C CXX)

option(USE_GLES "Force the use of OpenGL ES")

if(NOT ANDROID)
  add_executable(${PROJECT_NAME})
else()
  add_library(${PROJECT_NAME} SHARED)
  target_compile_definitions(${PROJECT_NAME} PRIVATE USE_JNI)
endif()

# == Dependencies ==
find_package(Git REQUIRED)
execute_process(COMMAND
  ${GIT_EXECUTABLE} submodule update --init --recursive
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("extlib/tinygltf" tinygltf EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} tinygltf)

add_subdirectory("extlib/entt" entt EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} EnTT::EnTT)

add_subdirectory("extlib/glm" glm EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} glm::glm)

target_include_directories(${PROJECT_NAME} PRIVATE "extlib/stb")

add_subdirectory("extlib/glad/cmake" glad_cmake EXCLUDE_FROM_ALL)
if(NOT ANDROID AND NOT USE_GLES)
  glad_add_library(glad REPRODUCIBLE API gl:core=3.3 EXTENSIONS NONE)
else()
  glad_add_library(glad REPRODUCIBLE API gles2=3.0 EXTENSIONS NONE)
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_GLES)
endif()
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory("extlib/sdl" sdl EXCLUDE_FROM_ALL)
if(NOT ANDROID)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)
else()
  set(SDL_STATIC OFF CACHE BOOL "" FORCE)
  set(SDL_SHARED ON CACHE BOOL "" FORCE)
  target_link_libraries(${PROJECT_NAME} SDL2)
endif()

# == Source Files ==
target_sources(${PROJECT_NAME} PRIVATE
  LICENSE.md
  README.md
  images/heightmap.png
  shaders/render.vert
  shaders/render.frag
  shaders/camera.vert
  shaders/camera.frag
  shaders/heightmap.vert
  shaders/heightmap.frag
  src/core/gl.hpp
  src/core/gl.cpp
  src/core/window.hpp
  src/core/window.cpp
  src/components/spatial.hpp
  src/components/camera.hpp
  src/components/camera.cpp
  src/components/heightmap.hpp
  src/components/heightmap.cpp
  src/components/render.hpp
  src/components/render.cpp
  src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "src")

# == Compiled-in Resources ==
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/resource")
include(resource)
add_resource(res
  CSTR
    shaders/render.vert
    shaders/render.frag
    shaders/camera.vert
    shaders/camera.frag
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
