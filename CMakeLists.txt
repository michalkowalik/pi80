cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pi80 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


## project config
add_executable(pi80 main.c
        pio_clock.c
        pio_clock.h)

pico_generate_pio_header(pi80 squarewave.pio)
target_sources(pi80 PRIVATE main.c)


# pull in common dependencies
target_link_libraries(pi80 pico_stdlib hardware_pio)
pico_add_extra_outputs(pi80)

# enable usb output, disable uart output
pico_enable_stdio_usb(pi80 1)
pico_enable_stdio_uart(pi80 0)

