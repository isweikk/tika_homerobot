cmake_minimum_required(VERSION 3.0.0)

set(DISP_TARGET disp)

# 包含目录
include_directories("./inc")
include_directories("./inc/SDL2")
include_directories("./inc/ffmpeg")

# 包含文件
set(DISP_SRC
    ./src/disp_texture.cpp
    ./src/disp_main_ui.cpp
#    ./src/disp_timer.cpp
)

# 链接库
set(STATIC_LIB
    
)

set(SHARED_LIB
    SDL2
    SDL2_image
    SDL2_ttf
    avformat
    avdevice
    avcodec
    avutil
    avfilter
    swresample
    postproc
    swscale
)

link_libraries(${SHARED_LIB})
add_executable(${DISP_TARGET}  disp_test.cpp ${DISP_SRC})

