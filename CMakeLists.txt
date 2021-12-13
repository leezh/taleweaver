cmake_minimum_required(VERSION 3.11)
project(taleweaver)

set(
  PROJECT_SOURCES
  src/main.c
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME})
