# m5stickc-mruby

Example application for running mruby on M5StickC. This is pretty much the same as https://github.com/mimaki/M5Stack-mruby but running against the [M5StickC](https://github.com/m5stack/M5StickC)
library instead of [M5Stack](https://github.com/m5stack/M5Stack). These libraries are very similar, to the extent that many of the same mrbgems can be used

This uses submodules for mruby, arduino-esp32  and M5StickC libraries - either use `git clone --recursive` or 

```
git submodule init
git submodule update
```

The esp-idf tools should be installed (and exported to the environment). This example has been tested against 3.3.1

# Getting started

(this is just standard esp-idf functionality)

- Run `make menuconfig` to customize the serial port device
- Run `make flash` to build bootloader, app and flash to device. Add `monitor` to tail logs from device (i.e. make flash monitor)

Edit components/mruby_component/esp32_build_config.rb to control the gems compiled into the app. The submodule at components/mruby_component/mruby controls the version of mruby used

main/main.rb is run when the device starts up

# IO

by default stderr go to the serial port (viewable with `make monitor`) and stdout goes to the buildin screen. This is provided by the mruby-stdio / mruby-stdio-m5stick gems
