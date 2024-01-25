cmake_minimum_required(VERSION 3.5)

include(pico-sdk/pico_sdk_init.cmake)

project(picotamachibi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(picotamachibi
    # Application
    main.cpp
    context.hpp context.cpp
    icon.hpp icon.cpp

    # Runtime data
    font.h font.c
    incbin.h
    filesystem.hpp filesystem.cpp
    microtar/src/microtar.h microtar/src/microtar.c

    # Drivers
    framebuffer.hpp framebuffer.cpp
    display.hpp display.cpp
    human_input.hpp human_input.cpp

    # Low level drivers
    ssd1306.h ssd1306.c
)

target_link_libraries(picotamachibi PUBLIC
    pico_stdlib hardware_uart hardware_i2c
)
target_compile_definitions(picotamachibi PRIVATE
    __unused=
    LIB_PICO_MULTICORE=0
    PICO_UART_ENABLE_CRLF_SUPPORT=0
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories(picotamachibi PUBLIC
    include/
)

pico_add_extra_outputs(picotamachibi)
#pico_enable_stdio_uart(picotamachibi 1)
#pico_enable_stdio_usb(picotamachibi 1)
