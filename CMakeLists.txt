cmake_minimum_required(VERSION 3.10)

project(CDS LANGUAGES C)

add_library(${PROJECT_NAME} SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src/cds.c")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(${PROJECT_NAME}packer "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
target_link_libraries(${PROJECT_NAME}packer PUBLIC ${PROJECT_NAME})