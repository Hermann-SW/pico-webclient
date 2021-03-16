# pico-webclient

This repo is based on [pico-webserver](https://github.com/maxnet/pico-webserver) webserver, which is example that came with TinyUSB slightly modified to run on a Raspberry Pi Pico.
Lets the Pico pretend to be a USB Ethernet device (192.168.7.1), the computer the Pico is connected to over USB is 192.168.7.2. For me that is a Pi400, replace Pi400 with whatever computer you connect your Pico to.

## Build dependencies

On Debian:

```
sudo apt install git build-essential cmake gcc-arm-none-eabi
```

Your Linux distribution does need to provide a recent CMake (3.13+).
If not, compile [CMake from source](https://cmake.org/download/#latest) first.

## Build instructions

```
git clone --depth 1 https://github.com/Hermann-SW/pico-webclient
cd pico-webclient
git submodule update --init --depth 1
mkdir -p build
cd build
cmake ..
make
```

Copy the resulting pico_webclient.uf2 file to the Pico mass storage device manually.
By default (SCENARIO=1 in CMakeLists.txt) Pico is accessing index.html on Pi400 webserver. In case you have no webserver running, you can start a simple one:
```
sudo ./do_get.py
```

