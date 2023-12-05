# 可选 指明最低cmake版本
cmake_minimum_required(VERSION 3.10.1)

# 设置C++17编译
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread")

# 设置解决方案名称
project(PalmLocalDemo)

# 变量添加
set(LIBRARY_NAME Palm)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/palm)
set(EXECUTABLE_NAME palm)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 打包所有源文件 下面两种方式都可以
# aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCES)
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# 设置外部库头文件变量
set(OpenCV_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include/opencv2)
set(EIGEN3_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include/Eigen)
set(CryptoPP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include/crypto++)

# 设置外部库动态库文件变量
file(GLOB OpenCV_LIBS ${CMAKE_SOURCE_DIR}/lib/opencv2/*.so*)
file(GLOB CryptoPP_LIBS ${CMAKE_SOURCE_DIR}/lib/crypto++/*.so*)

# 生成动态库 动态库选项 SHARED
add_library(${LIBRARY_NAME} SHARED ${SOURCES})

# 指定动态库的头文件搜索路径
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/PalmDemo)
target_include_directories(${LIBRARY_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${LIBRARY_NAME} PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(${LIBRARY_NAME} PUBLIC ${CryptoPP_INCLUDE_DIRS})

# 链接动态库到动态库
foreach(lib ${OpenCV_LIBS} ${CryptoPP_LIBS})
    target_link_libraries(${LIBRARY_NAME} ${lib})
endforeach()

# 将生成的动态库设置为变量 Palm_LIBS
file(GLOB Palm_LIBS ${CMAKE_SOURCE_DIR}/lib/palm/*.so*)

# 生成可执行文件
add_executable(${EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/main/main.cpp)

# 指定可执行文件的头文件搜索路径
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/PalmDemo)
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CryptoPP_INCLUDE_DIRS})

# 链接动态库到可执行文件
foreach(lib ${OpenCV_LIBS} ${CryptoPP_LIBS} ${Palm_LIBS})
    target_link_libraries(${EXECUTABLE_NAME} ${lib})
endforeach()