# FortuneTeller

this is a work in progress repository for a fortune telling machine that i'm building from scratch. this version uses the pimoroni servo2040 (i later decided to switch to the pimoroni yukon to add functionality) and also includes so basic facial recognition code so the maching can hand out person specific cards for a later add on.

see the yukon octopus repository for the latest code.

## getting started using the pimoroni servo2040

this section includes instructions for getting the build environment for the servo2040 up and running

### pimoroni servo2040 initialization procedures

1. somewhere in git directory run ```git submodule update --init --recursive```
2. set the path to the pico-sdk ```export PICO_SDK_PATH=/path/to/FortuneTeller/octopus/pico-sdk```
3. prepare for installing toolchain using `sudo apt update` then `sudo apt upgrade`
4. install toolchain ```sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential```
5. clean up with ```sudo apt get clean```

### build and flashing procedures

1. create working build directory in the octopus filder using ```mkdir /path/to/FortuneTeller/octopus/servo/build``` (**/build is in gitignore)
2. navigate to build folder and then use ```cmake ..``` to create cmake build directory in the build folder
3. now use ```make``` to build application and generate uf2 file
3. boot rp2040 in boot/user mode and flash with octopus.uf2 by dragging into RPI-RP2 folder

## getting started with facial recognition

this section includes instructions for getting 

### opencv initialization procedures

1. setup python venv ```python -m venv /path/to/FortuneTeller/vision/venv```
2. activate venv ```source /path/to/FortuneTeller/vision/venv/bin/activate```
3. install packages in requirements.txt ```pip install -r path/to/FortuneTeller/requirements.txt```

note that the requirements.txt includes opencv-python-headless which is not strictly necessary but is useful for codespaces or other environments which might not have necessary dependencies installed. when using such environments you should separately ensure the venv created above is the python interpreter in use.

### facial recognition workflow procedures

1. place short videos of friends (i.e. live photos) in the training-videos folder
2. run generate-data.py to generate training-data for the opencv facial recognition to use
3. train the algorithm in train-faces.py on the training data
4. (unimplemented) write code to see if live photo of machine operator is in database
5. (unimplemented) eject card paired to that user or random card otherwise


