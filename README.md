# Color Blind Helper

Color Blind Helper is an electronic device designed to assist people with color vision deficiency in recognizing colors. The identified color is displayed on an I2C OLED display.

## Description

The project uses an Arduino board (NANO) and a color sensor (such as TCS3200 or TCS34725) to detect and display the color name on an OLED I2C display. There are no audio feedback or other output types besides the OLED display (though these could be added in future versions).

## Requirements

### Hardware

- Arduino (Nano preferred, but others can be used)
- Color sensor (e.g., TCS3200, TCS34725)
- I2C OLED display (e.g., 0.96'' SSD1306)
- Jumper wires and breadboard as needed

### Example Connection with Arduino NANO

**Power Supply and Saving Battery:** The device can be powered using a 9V battery. To minimize battery consumption, a push button is used to supply voltage to the VIN pin of Arduino only when the button is pressed. This way, the device is powered on solely while in use.

**Pull-up resistors:** If both the GY-33 and the OLED display already have pull-up resistors on SDA/SCL, adding extra is usually unnecessary. If there are communication issues, try adding 4.7kΩ pull-up resistors to VCC.

| Component                  | Relevant Pins                            | Notes                                                      |
|----------------------------|------------------------------------------|------------------------------------------------------------|
| **Arduino Nano**           | SDA (A4), SCL (A5), VCC (5V/3.3V), GND  | Main microcontroller                                       |
| **OLED I2C Display**       | SDA, SCL, VCC, GND                       | Typical address `0x3C`                                     |
| **GY-33 Sensor (TCS34725)**| SDA, SCL, VCC, GND                       | Based on TCS34725, typical address `0x29`                  |
| **Pull-up Resistors**      | 4.7kΩ on SDA and SCL                     | From VCC to SDA and SCL, only if missing on modules        |

### Software

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- Drivers for your specific Arduino board
- Libraries for the color sensor and OLED display (see `platformio.ini`)

## Installation and Usage

1. **Clone** the repository to your local machine.
2. **Open** the project folder in VSCode.
3. **Ensure** that PlatformIO is installed as an extension.
4. **Install** the libraries specified in `platformio.ini` (these are installed automatically).
5. **Assemble** the device according to the diagram (connect the color sensor and OLED I2C display to Arduino).
6. **Build** and **upload** the firmware via PlatformIO (`Ctrl+Alt+U`).
7. **Power on and use:** the detected color will be shown on the OLED display.

## Acknowledgements

Special thanks to the Arduino community and the developers of the libraries used in this project.

A further thank you to the creators of the tool [image2cpp](https://javl.github.io/image2cpp/), which greatly facilitated the conversion of bitmaps for use with (monochrome) displays in Arduino projects.


**PS:** I wrote this README with AI because I didn’t feel like writing it by hand. Sorry