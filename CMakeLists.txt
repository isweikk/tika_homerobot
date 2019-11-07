cmake_minimum_required(VERSION 3.0.0)

project(glmedia VERSION 0.1.0)


MESSAGE(STATUS "current platform: Linux ")
set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER   gcc)

#add_definitions(-DOS_DEBUG)

set(TARGET tika_center)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_compile_options(-std=c++11)

# 包含目录
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
include_directories("${PROJECT_SOURCE_DIR}/lib/SDL2")
include_directories("/usr/include/c++/8/")


link_directories("${PROJECT_SOURCE_DIR}/lib")

# 链接库
set(STATIC_LIB
    
)

set(SHARED_LIB

)

#link_libraries(${SHARED_LIB})
#add_executable(${TARGET}  test/test.cpp)

# 包含子项目。
add_subdirectory ("src/disp")
