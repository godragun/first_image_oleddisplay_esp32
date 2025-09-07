# first_image_oleddisplay_esp32

This project demonstrates how to display the first image on an OLED display using an ESP32 microcontroller.

## Overview

This repository contains sample code and resources for connecting an ESP32 board to an OLED display and rendering an initial image. It is intended as a starting point for beginners or anyone looking to integrate OLED display functionality with their ESP32 projects.

## Hardware Requirements

- ESP32 development board
- OLED display (e.g., SSD1306 128x64 via I2C or SPI)
- Jumper wires
- Breadboard (optional)

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
- ESP32 board support installed in Arduino IDE/PlatformIO
- `Adafruit_SSD1306` and `Adafruit_GFX` libraries (install via Library Manager)

## Wiring

| OLED Pin | ESP32 Pin (Example) |
|----------|--------------------|
| VCC      | 3.3V               |
| GND      | GND                |
| SCL/SCK  | GPIO 22 (SCL)      |
| SDA      | GPIO 21 (SDA)      |

> _Note: Double-check pin assignments with your ESP32 board and OLED display datasheet._

## Usage

1. Clone this repository:
    ```sh
    git clone https://github.com/godragun/first_image_oleddisplay_esp32.git
    ```
2. Open the project in your IDE.
3. Install the required libraries.
4. Connect your ESP32 and OLED as described above.
5. Upload the code to your ESP32.
6. The OLED display should show the default image after boot.

## Example Code

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  // Draw a bitmap or image here
  display.drawPixel(64, 32, SSD1306_WHITE); // Example: single pixel
  display.display();
}

void loop() {
  // Nothing here
}
```

## Custom Images

You can convert images to byte arrays using tools like [LCD Image Converter](https://lcd-image-converter.riuson.com/en/about/) and display them with `display.drawBitmap()`.

## License

This project is licensed under the MIT License.

---

Feel free to contribute or open issues for improvements!
