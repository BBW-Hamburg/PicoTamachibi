cmake_minimum_required(VERSION 3.5)

option(BUILD_EMULATOR "Build emulator instead" Yes)

if (NOT BUILD_EMULATOR)
    include(pico-sdk/pico_sdk_init.cmake)
endif()

project(picotamachibi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (BUILD_EMULATOR)
    set(DRIVER_SOURCES
        emulator/emulator.h
        ssd1306.h emulator/ssd1306.c
        emulator/gpio.c
        emulator/emulator.cpp
    )

    add_subdirectory(minifb)
else()
    pico_sdk_init()

    set(DRIVER_SOURCES
        ssd1306.h ssd1306.c
    )
endif()

add_executable(picotamachibi
    # Application
    main.cpp
    context.hpp context.cpp
    icon.hpp icon.cpp
    event.hpp event.cpp

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
    ${DRIVER_SOURCES}
)

target_include_directories(picotamachibi PUBLIC
    include/
)
target_compile_definitions(picotamachibi PRIVATE
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if (BUILD_EMULATOR)
    target_link_libraries(picotamachibi PRIVATE minifb)
    target_include_directories(picotamachibi PUBLIC
        include/emulator/
    )
    target_compile_options(picotamachibi PUBLIC -fno-exceptions)
    target_link_options(picotamachibi PUBLIC -fno-exceptions)
else()
    target_link_libraries(picotamachibi PUBLIC
        pico_stdlib hardware_uart hardware_i2c
    )
    target_compile_definitions(picotamachibi PRIVATE
        __unused=
        LIB_PICO_MULTICORE=0
        PICO_UART_ENABLE_CRLF_SUPPORT=0
    )

    pico_add_extra_outputs(picotamachibi)
    #pico_enable_stdio_uart(picotamachibi 1)
    #pico_enable_stdio_usb(picotamachibi 1)
endif()
