cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pi80 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


## project config
add_executable(pi80 main.c
        pio_handlers.c
        pio_handlers.h
        pins.h)

pico_generate_pio_header(pi80 ${CMAKE_CURRENT_LIST_DIR}/databus.pio)
pico_generate_pio_header(pi80 ${CMAKE_CURRENT_LIST_DIR}/addressbus.pio)
pico_generate_pio_header(pi80 ${CMAKE_CURRENT_LIST_DIR}/clock.pio)
target_sources(pi80 PRIVATE main.c)


# pull in common dependencies
target_link_libraries(pi80 pico_stdlib hardware_pio)
pico_add_extra_outputs(pi80)

# enable usb output, disable uart output
pico_enable_stdio_usb(pi80 1)
pico_enable_stdio_uart(pi80 0)

