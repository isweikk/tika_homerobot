cmake_minimum_required(VERSION 3.0.0)

project(glmedia VERSION 0.1.0)


set(CMAKE_SYSTEM_NAME "Windows")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_C_COMPILER   gcc)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
    add_definitions(-DWIN32 -DOS_EXPORTS -D_WINSOCK_DEPRECATED_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MDd")

ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

#add_definitions(-DOS_DEBUG)
set(TARGET gvideo)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_compile_options(-std=c++11)

# 包含目录
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/lib")
include_directories("${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
include_directories("/mnt/c/mingw-w64/mingw64/lib/gcc/x86_64-w64-mingw32/8.1.0/include/c++")


link_directories("${PROJECT_SOURCE_DIR}/lib")
link_directories("${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib")

# 链接库
set(STATIC_LIB
    jrtplib
    jthread
)

set(SHARED_LIB
    SDL2
    avformat
    avdevice
    avcodec
    avutil
    avfilter
    swresample
    postproc
    swscale
)
add_library(${TARGET} SHARED src/rtsp_client.cpp src/decoder.cpp  src/disp.cpp src/gvideo_api src/sha1.cpp src/base64.cpp src/websocket_codetool.cpp src/websocket_api.cpp)
#add_library(${TARGET}_static STATIC src/rtsp_client.cpp src/decoder.cpp  src/disp.cpp src/gvideo_api)
target_link_libraries(${TARGET} ${STATIC_LIB} ${SHARED_LIB})
#target_link_libraries(${TARGET}_static ${STATIC_LIB} ${SHARED_LIB})

# 指定静态库的输出名称
#set_target_properties(${TARGET}_static PROPERTIES OUTPUT_NAME ${TARGET}_st)
# 使动态库和静态库同时存在
#set_target_properties(${TARGET} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#set_target_properties(${TARGET}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)



add_executable(${TARGET}_test  src/video_test.cpp)
#target_link_libraries(${TARGET}_test ${STATIC_LIB} ${SHARED_LIB})
target_link_libraries(${TARGET}_test  ${TARGET}.dll)

# 包含子项目。
#add_subdirectory ("src")
