cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pi80 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

SET(PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK 1)

pico_sdk_init()


## project config
add_executable(pi80 main.c
        pio/pio_handlers.c
        pio/pio_handlers.h
        pins.h
        memory/memory.h
        memory/memory.c
        boot_loader.h
        slow_clock.c
        slow_clock.h
        rom_data/basic.h
        io_operations.c)

pico_generate_pio_header(pi80 ${CMAKE_CURRENT_LIST_DIR}/pio/databus.pio)
pico_generate_pio_header(pi80 ${CMAKE_CURRENT_LIST_DIR}/pio/addressbus.pio)
pico_generate_pio_header(pi80 ${CMAKE_CURRENT_LIST_DIR}/pio/clock.pio)
target_sources(pi80 PRIVATE main.c)


# pull in common dependencies
target_link_libraries(pi80 pico_stdlib hardware_pio)
pico_add_extra_outputs(pi80)

# enable usb output, disable uart output
pico_enable_stdio_usb(pi80 1)
pico_enable_stdio_uart(pi80 0)
target_compile_definitions(pi80 PRIVATE
        PICO_DEFAULT_UART_TX_PIN=16
        PICO_DEFAULT_UART_RX_PIN=17
)