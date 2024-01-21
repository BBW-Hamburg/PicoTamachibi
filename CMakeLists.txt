cmake_minimum_required(VERSION 3.5)

include(pico-sdk/pico_sdk_init.cmake)

project(picoswitch LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(picoswitch
    main.cpp
)

target_link_libraries(picoswitch PUBLIC
    pico_stdlib hardware_uart
)
target_compile_definitions(picoswitch PRIVATE
    __unused=
    LIB_PICO_MULTICORE=0
    PICO_UART_ENABLE_CRLF_SUPPORT=0
    ${CONSOLE_DEFINES}
)
target_include_directories(picoswitch PUBLIC
    include/
)

pico_add_extra_outputs(picoswitch)
#pico_enable_stdio_uart(picoswitch 1)
#pico_enable_stdio_usb(picoswitch 1)
