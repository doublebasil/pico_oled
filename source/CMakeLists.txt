add_executable(${PROJECT_NAME}
    main.cpp
    oled/oled.cpp
    oled/intcos.cpp
    QR-Code-generator/qrcodegen.c
    )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SPI-Rpi-Pico/FatFs_SPI build-fatfs-spi-pico)

# Adding a directory here means anything within this project can include it
# Might be the wrong way to do it but I couldn't figure out the right way
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/oled
    ${CMAKE_CURRENT_LIST_DIR}/oled/font

    ${CMAKE_CURRENT_LIST_DIR}/QR-Code-generator
    )

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_spi

    FatFs_SPI
    )

# Enable USB and UART for serial output
pico_enable_stdio_usb(${PROJECT_NAME} TRUE)
pico_enable_stdio_uart(${PROJECT_NAME} TRUE)

# By default make only produces a .elf, this function allows uf2 and others to be generated
pico_add_extra_outputs(${PROJECT_NAME})
