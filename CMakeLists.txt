cmake_minimum_required(VERSION 3.13)

# We prefer to have all linked submodules at toplevel
set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb)

include(pico-sdk/pico_sdk_init.cmake)
project(pico_webclient)
pico_sdk_init()

# LWIP
set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lwip)
set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
include(${LWIP_DIR}/src/Filelists.cmake)

# Extra stuff from TinyUSB, that is not part of tinyusb_device library
set(TINYUSB_LIBNETWORKING_SOURCES
    ${PICO_TINYUSB_PATH}/lib/networking/dhserver.c
    ${PICO_TINYUSB_PATH}/lib/networking/rndis_reports.c
)

add_executable(${PROJECT_NAME} webclient.c tusb_lwip_glue.c usb_descriptors.c ${TINYUSB_LIBNETWORKING_SOURCES})

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
target_include_directories(${PROJECT_NAME} PRIVATE ${LWIP_INCLUDE_DIRS} ${PICO_TINYUSB_PATH}/src ${PICO_TINYUSB_PATH}/lib/networking)
target_link_libraries(${PROJECT_NAME} pico_stdlib pico_unique_id tinyusb_device lwipcore)
pico_add_extra_outputs(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE SCENARIO=1)
