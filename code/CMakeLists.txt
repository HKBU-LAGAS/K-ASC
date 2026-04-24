# 1. 基本项目设置
cmake_minimum_required(VERSION 3.5) # 建议提高版本以使用更现代的CMake功能
project(swing CXX) # 明确指定语言为 CXX

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录 (这是一个好习惯)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 2. 查找通用依赖项
find_package(Boost REQUIRED COMPONENTS filesystem system date_time program_options)
find_package(Threads REQUIRED)


# 3. 设置 QFilter 的依赖项 (METIS)
# 使用 find_library 和 find_path 来更健壮地查找 METIS
#set(METIS_ROOT_DIR "/home/comp/csrhjiang/local" CACHE PATH "Root directory of METIS installation")
#find_path(METIS_INCLUDE_DIR metis.h HINTS "${METIS_ROOT_DIR}/include")
#find_library(METIS_LIBRARY metis HINTS "${METIS_ROOT_DIR}/lib" "${METIS_ROOT_DIR}/lib64")
#find_library(GKLIB_LIBRARY GKlib HINTS "${METIS_ROOT_DIR}/lib" "${METIS_ROOT_DIR}/lib64")
#
#if(NOT METIS_INCLUDE_DIR OR NOT METIS_LIBRARY)
#    message(FATAL_ERROR "METIS not found! Please set METIS_ROOT_DIR correctly.")
#endif()
#message(STATUS "Found METIS include dir: ${METIS_INCLUDE_DIR}")
#message(STATUS "Found METIS library: ${METIS_LIBRARY}")

# 4. 创建 QFilter 功能库 (静态库)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(QFILTER_DIR ${PROJECT_INCLUDE_DIR}/QFilter)
add_library(qfilter_lib STATIC
        ${QFILTER_DIR}/util.cpp
        ${QFILTER_DIR}/set_operation.cpp
        ${QFILTER_DIR}/bitpack_maximal_clique.cpp
)
# 为 qfilter_lib 设置属性 (头文件路径、编译选项)
target_include_directories(qfilter_lib PUBLIC
        ${PROJECT_INCLUDE_DIR}
#        ${METIS_INCLUDE_DIR}
)
target_compile_options(qfilter_lib PRIVATE -O3 -Wall -Wno-unused-result -mavx2 -Wno-misleading-indentation)

# 5. 创建 swing 项目原有的库 (修正版)
# 修正: add_library 只接受源文件 (.cc)，不接受头文件 (.h)
add_library(algo STATIC src/algo.cc)
add_library(graph STATIC src/graph.cc)
add_library(utils STATIC src/utils.cc)

# 为 swing 的库设置属性
set(SWING_COMPILE_FLAGS "-O3" "-w") # -w 抑制所有警告，建议用 -Wall 进行修复
foreach(lib algo graph utils)
    target_include_directories(${lib} PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_compile_options(${lib} PRIVATE ${SWING_COMPILE_FLAGS})
endforeach()

target_link_libraries(algo PUBLIC qfilter_lib)

# 6. 创建最终的可执行文件
add_executable(main src/main.cc)

# 为 main 添加头文件路径
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 链接所有需要的库 (现代方式)
target_link_libraries(main PRIVATE
        # swing 自身的库
        graph
        algo
        utils

#        # Dothash 库
#        dothash

        # 新集成的 QFilter 库
        qfilter_lib

        # QFilter 的外部依赖
#        ${METIS_LIBRARY}
#        ${GKLIB_LIBRARY}
        m # 数学库

        # swing 的外部依赖 (使用现代 CMake targets)
        Boost::program_options
        Boost::filesystem
        Boost::system
        stdc++fs

        # 通用依赖
        Threads::Threads
        dl
        rt
)

# 7. 将可执行文件复制到项目根目录 (可选)
# 这个命令会在每次构建 main 之后，将生成的可执行文件复制到项目根目录
add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:main>
        ${PROJECT_SOURCE_DIR}/main
        COMMENT "Copying executable to project root directory"
)