cmake_minimum_required(VERSION 3.5)

project(many-elements LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(VTK 9.0 REQUIRED NO_MODULE)

set (
    HEADERS
)

set (
    SOURCES
    main.cxx
)

message(STATUS "libs: ${VTK_LIBRARIES}")

add_executable(many-elements ${SOURCES} ${HEADERS})

target_link_libraries(
    many-elements
    ${VTK_LIBRARIES}
)
