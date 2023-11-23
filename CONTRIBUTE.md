# Install IDE
The P1-wifi-gateway can be built with either PlatformIO or Arduino 2.

## PlatformIO

### Installation
https://docs.platformio.org/en/latest/integration/ide/pioide.html
- VSCode
- CLion (IntelliJ)
- Commandline

### Configuration
Within platformIO all the configuration is stored within `platform.ini`. Therefore, no additional steps are needed before the actual build.

### Build
 - VSCode: Hit the `build` button
 - CLion: Hit the `build` button
 - Commandline: `platformio run --environment espmxdevkit`

The build is places in .pio/build/espmxdevkit/firmware.bin 

### Arduino 2

### Installation
https://www.arduino.cc/en/software

### Configuration
#### Add Board manager
In the preferences of Arduino IDE add the following board manager url

> https://arduino.esp8266.com/stable/package_esp8266com_index.json

### Install the ESP8266 board
On the left panel in the "Board manager" install

- `esp866` by ESP8266 community | current version 3.1.2

### Install the required libraries

- `Time` by Michael Margolis | version 1.6.1 (search for TimeLib)
- `MyAlarm` by z01ne | version 1.1.2
- `PubSubClient` by Nick O'Leary | version 2.8

### Select the correct board
Search for `DOIT ESP-Mx DevKit (ESP8285)`

### Build
Click `Sketch > verify/compile`. The binary is places in the temporary folder

- Linux on `/tmp/arduino/sketches/`
- Mac on ???
- Windows on ???

# Test
- Commandline: `platformio test --environment test`

# Upload the new firmware
By default, the P1-wifi-gateway cannot be connected via USB. Therefore, the only option to install the new firmware is by uploading the binary in the Webinterface.