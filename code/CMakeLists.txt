cmake_minimum_required(VERSION 3.5)
project(swing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS filesystem system date_time program_options)
find_package(Threads REQUIRED)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(QFILTER_DIR ${PROJECT_INCLUDE_DIR}/QFilter)
add_library(qfilter_lib STATIC
        ${QFILTER_DIR}/util.cpp
        ${QFILTER_DIR}/set_operation.cpp
        ${QFILTER_DIR}/bitpack_maximal_clique.cpp
)
target_include_directories(qfilter_lib PUBLIC
        ${PROJECT_INCLUDE_DIR}
)
target_compile_options(qfilter_lib PRIVATE -O3 -Wall -Wno-unused-result -mavx2 -Wno-misleading-indentation)

add_library(algo STATIC src/algo.cc)
add_library(graph STATIC src/graph.cc)
add_library(utils STATIC src/utils.cc)

set(SWING_COMPILE_FLAGS "-O3" "-w")
foreach(lib algo graph utils)
    target_include_directories(${lib} PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_compile_options(${lib} PRIVATE ${SWING_COMPILE_FLAGS})
endforeach()

target_link_libraries(algo PUBLIC qfilter_lib)

add_executable(main src/main.cc)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(main PRIVATE
        graph
        algo
        utils
        qfilter_lib
        Boost::program_options
        Boost::filesystem
        Boost::system
        stdc++fs
        Threads::Threads
        dl
        rt
)

add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:main>
        ${PROJECT_SOURCE_DIR}/main
        COMMENT "Copying executable to project root directory"
)