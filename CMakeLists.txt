
cmake_minimum_required(VERSION 3.15)

project(frp_proj)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_GENERATOR MATCHES Ninja)
    add_compile_options(-fdiagnostics-color=always)
endif()

### dependencies ###

# catch2
add_library(frp_external::Catch2 INTERFACE IMPORTED)
set_target_properties(frp_external::Catch2 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/catch2/include"
    )

### project ###

add_library(frp INTERFACE IMPORTED)
set_target_properties(frp PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/include"
    )

add_subdirectory(tests)
