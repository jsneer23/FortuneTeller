cmake_minimum_required(VERSION 3.12)

# Set executable name
set(NAME octopus) 

include(pico_sdk_import.cmake)
include(pimoroni_pico_import.cmake)


project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${NAME}
    servo/main.cpp
    servo/servo.cpp
)

# Include Libraries
include(drivers/button/button)
include(drivers/analog/analog)
include(drivers/analogmux/analogmux)
include(libraries/servo2040/servo2040)

target_link_libraries(${NAME} 
    pico_stdlib
    pico_multicore
    servo2040 
    button 
    analog 
    analogmux
    )

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)