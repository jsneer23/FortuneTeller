# FortuneTeller

### Initialization Procedures

1. somewhere in git directory run `git submodule update --init --recursive`
2. set the path to the pico-sdk `export PICO_SDK_PATH=/workspaces/FortuneTeller/pico-sdk` (or local path if not workspace)
3. install the toolchain using `sudo apt update` then `sudo apt upgrade` and finally `sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential`

### Compile and Flashing Procedures

1. create working directory using `mkdir build` (**/build is in gitignore)
2. navigate to build folder and then `cmake ..` to create cmake build directory
3. now `make` to build application and generate uf2 file
3. boot rp2040 in boot/user mode and flash with octopus.uf2 by dragging into RPI-RP2 folder