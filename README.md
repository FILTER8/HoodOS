# HoodOS

Open-source experimental firmware for OnChainHoodies on an ESP32-S3 with 240 x 240 ST7789 LCD and CST816 touchscreen.

## v0.1 features

- Strict two-color interface: black `#000000` and green `#CCFF00`
- Touchscreen navigation: HOOD / BANKR / SETUP
- Wi-Fi connection and connection status
- No blockchain transactions, wallet keys, or voice commands in this release

## Hardware

This build targets the **specific board wiring** from the manufacturer's working `09_lvgl_arduino_v9.ino` example, not all ESP32-S3 boards. Display: DC 45, CS 21, SCK 38, MOSI 39, RST 40, backlight 46. Touch: SDA 42, SCL 41, IRQ 48, RST 47.

## Install (Arduino IDE)

1. Install ESP32 Arduino board support and select the appropriate ESP32-S3 board profile for your device.
2. Install **LVGL 9.x**, **Arduino_GFX_Library**, and the manufacturer's **SensorLib / TouchDrvCSTXXX** dependency (same libraries used by your working example).
3. Copy `firmware/HoodOS/secrets.example.h` to `firmware/HoodOS/secrets.h`.
4. Edit `secrets.h` with your local 2.4 GHz Wi-Fi network name and password. **Never commit this file.**
5. Open `firmware/HoodOS/HoodOS.ino`, compile and upload to the device.
6. Open Serial Monitor at 115200 baud if debugging is needed.

## Current limitations

- Wi-Fi credentials are entered at compile time. On-device provisioning is planned.
- The HOOD and BANKR pages are placeholders; no API calls are made.
- Audio/microphone and wallet transaction authorization are not implemented.
- Touchscreen behavior follows the vendor's IRQ-driven example and must be tested on the actual hardware.
- UI colors are black/green; physical LCD output uses RGB565 and may approximate `#CCFF00`.

## Roadmap

- v0.2: OnChainHoodies pixel API and real Hoodie avatar
- v0.3: Secure voice gateway and Bankr integration
- v0.4: Device setup/provisioning and optional audio feedback

## Security

Never store a wallet private key, Bankr API secret, or Cloudflare secret in public firmware. Any future financial actions must use authenticated backend requests and explicit transaction approval.

## Publishing

Create a new GitHub repository named `HoodOS` and push this folder. The `.gitignore` excludes local Wi-Fi credentials.
