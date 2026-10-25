## Memory fix in v0.3.2 (for esp-sha allocation errors)

The ESP32-S3 was allocating a 57,600-byte LVGL draw/flush buffer in **internal** RAM even though the full-screen canvas lives in PSRAM. This version uses a 14,400-byte 30-row partial draw buffer (saving **43,200 bytes** of internal RAM). The displayed 240x240 artwork and RGB565 quality are unchanged.

- **Pixel fetch is OFF by default** (`HOODOS_ENABLE_PIXEL_FETCH 0`). Test Wi-Fi for 20 seconds first, then change it to `1` and upload a second time to test HTTPS.
- Settings now shows `RAM ... MAX ...` (the largest allocatable internal-RAM block), `PSRAM OK` or `NO PSRAM`, and the last Wi-Fi `ERR` disconnect code.
- Startup logs also show internal free memory, largest allocation and available PSRAM. If the Arduino Serial Monitor shows only `E (...) esp-sha: ...`, try **Tools -> USB CDC On Boot -> Enabled**, verify the correct USB port, open Serial Monitor at 115200 baud, and reconnect USB. The board's USB mode must match the selected port.
- Keep **Tools -> PSRAM -> OPI PSRAM**. A 115,200-byte RGB565 screen stored in internal memory rather than PSRAM can still starve network operations.

**Troubleshooting sequence:** With pixel fetch set to `0`, connect to iPhone hotspot and open Settings. Report the exact `WIFI`, `RAM ... MAX ...`, `PSRAM ... ERR ...` lines. Only enable fetch when Wi-Fi is stable. `esp-sha` alone indicates a memory-allocation failure, not necessarily an HTTP request: Wi-Fi authentication also uses crypto.

---

# HoodOS v0.3.2 — Live Hoodie + swipeable app launcher

**A tiny Tamagotchi-like companion for OnChainHoodies.** Built for the Waveshare ESP32-S3-Touch-LCD-1.54 (240x240 ST7789 + CST816 touchscreen).

## Wi-Fi / iPhone hotspot diagnostic fix in v0.3.1

- Turns ESP32 Wi-Fi sleep OFF (`WiFi.setSleep(false)`), which may help with hotspot stability.
- Logs Wi-Fi association, IP acquisition and disconnect reason to Serial Monitor (115200 baud).
- Waits 10 seconds after getting connected before making any HTTPS request.
- Prints reset reason on boot and free heap before/after HTTPS to identify crash/restart during TLS.
- `HOODOS_ENABLE_PIXEL_FETCH` at the top of `HoodOS.ino` is **0 by default**. If the device connects and then disconnects during the HTTPS request, change it to **0** and upload again. This disables ONLY pixel fetching, not Wi-Fi, display or swipes. If Wi-Fi then stays connected, the API/TLS path is implicated.
- The 10-second delay is intentional; the portrait will appear later than in v0.3.

The previous v0.3 may have been restarting during HTTPS initialization; that is a hypothesis, not yet proven. Send Serial Monitor output if it still disconnects. An iPhone hotspot showing `1` device only briefly is insufficient to distinguish disconnects from resets.

## New in v0.3

- Fetches the actual followed Hoodie's immutable 20x20 art from `https://api.onchainhoodies.xyz/v1/token/{id}/pixels` over Wi-Fi.
- Renders all 400 original pixels in exactly two colors: black and #CCFF00 (converted to RGB565 on the LCD). Black bits are ink; clear bits are lime background.
- Home page: huge **140x140 pixel** Hoodie portrait, followed ID and connection/load status.
- **Horizontal touch swipes** between HOME / APPS / MORE. The old cramped six-icon launcher is gone.
- APPS and MORE use large 2x2 touch tiles and original Smallbits 8x8 icons at 5x scale.
- Follow any ID 0–5999 from Settings or MORE; save it across power cycles. Changing the ID automatically fetches its art.
- Pixel sync includes HTTP response status checks, exact 100-character hex encoding, dimensions, encoding type, token ID, and black-pixel count validation, with retry after network failures.
- Retains the **working v0.2.1 display setup**, RGB565 canvas, PSRAM fallback, Wi-Fi setup, and local secrets.h format.
- Bankr, Journey, Games and Gallery remain placeholders. **Following is not ownership or authorization.**

## Installation: update the EXISTING HoodOS repository

1. Download `HoodOS-v0.3.zip` and extract it in your existing project root (the folder that contains `firmware/HoodOS/`). Do not delete your local `secrets.h`.
2. Open `firmware/HoodOS/HoodOS.ino` with Arduino IDE.
3. Keep your known-working board setup: **ESP32S3 Dev Module**, **Espressif core 3.2.0**, 16 MB flash, **OPI PSRAM**, **LVGL 9.x**, Arduino_GFX and Waveshare's installed `TouchDrvCSTXXX.hpp`/SensorLib. `LV_USE_CANVAS` must be enabled in `lv_conf.h`.
4. **Verify** first, then Upload and open the Serial Monitor at **115200 baud** if needed.
5. The Wi-Fi network must have **2.4 GHz** service. HoodOS first shows a black/lime placeholder, then downloads and displays the saved Hoodie as soon as Wi-Fi connects.
6. Touch: **swipe left** from HOME to APPS, then MORE; **swipe right** to return. On APPS/MORE, tap a big icon. To change Hoodie, open MORE -> FOLLOW, or SETTINGS -> CHANGE HOODIE, then enter the numeric ID and SAVE.

### Expected Serial Monitor

```text
HoodOS v0.3.2 boot
HoodTheme: full-screen RGB565 canvas created
HoodAPI: following #351
HoodWiFi: GOT IP 172.x.x.x
HoodOS Wi-Fi: ONLINE
(10 second stabilization period)
HoodAPI: fetching Hoodie #351
HoodAPI: Hoodie #351 loaded, 140 black pixels
```

Some ordering of the initial HoodAPI/Canvas messages may differ. The pixel endpoint already has to contain the selected token's JSON (`api/v1/pixels/{id}.json` in Cloudflare R2). If only three sample IDs are uploaded, any other ID will show `HTTP ERROR 404` until the full import finishes.

### Errors on-screen / Serial

- **WAITING FOR WIFI**: ESP32 cannot reach your 2.4 GHz network yet.
- **HTTP ERROR 404**: pixel JSON for selected token not uploaded (or Worker route not live).
- **HTTP ERROR -1/-11/...**: HTTPS, DNS, timeout or network issue; see Serial.
- **INVALID PIXEL DATA**: response did not match the fixed 20x20 bitmap format.
- Failures retry automatically after 15 seconds. Settings -> RETRY PIXELS retries immediately.

## Source layout

```text
firmware/HoodOS/
  HoodOS.ino             # Vendor-proven display/touch setup, LVGL loop
  secrets.example.h      # Sample Wi-Fi config; real secrets.h is ignored by git
  core/
    HoodAPI.h            # Fetch, validate, cache and draw 20x20 bitmap
    HoodNavigation.h     # Screen dispatch, gestures, redraw
    HoodState.h          # Persistent followed Hoodie + navigation state
    HoodTheme.h          # Two-color canvas + pixel rectangles
    HoodUICommon.h       # Header, controls, page dots
    HoodWiFi.h           # Existing Wi-Fi connection
  apps/
    HomeApp.h            # Full-screen companion home
    AppsApp.h            # Big swipeable app launchers
    HoodieApp.h          # Selected Hoodie and follow action
    BankrApp.h           # Placeholder
    JourneyApp.h         # Placeholder
    GamesApp.h           # Placeholder
    GalleryApp.h         # Placeholder
    SettingsApp.h        # Wi-Fi and followed ID; Retry Pixels
    FollowApp.h          # Numeric token picker
  assets/
    HoodFont.h + HoodFontData.h
    HoodIcons.h + smallbits_icons.h
```

For adding a new app, see `docs/ADDING_AN_APP.md`.

## Privacy and security

The pixel API endpoint is **public, read-only metadata** and is currently fetched over HTTPS with `WiFiClientSecure::setInsecure()` for prototype compatibility. **This skips TLS certificate verification** and is not appropriate for secure wallet actions or sensitive endpoints. A future production revision must install/pin an appropriate trusted CA or otherwise verify the server identity. **Never reuse this insecure client for Bankr or transactions.** No wallet signing, Bankr credentials or private keys are stored in this firmware.

## Known limitations

- This code was checked against the previously working v0.2.1 source, but cannot be physically compiled/flashed from this environment against your installed Arduino libraries. **Test the actual display, swipes, and HTTP fetching on your board before publishing.**
- HTTPS fetch currently runs synchronously with a 3.5 second connection/read timeout and can briefly pause touch feedback during a retry. Moving network requests to a separate task is a later improvement.
- Native CST816 swipe capture is device-dependent. v0.3 samples while the touch is down, but swipe threshold/direction may need adjustment after the first hardware test.
- Doesn't cache the bitmap across power cycles yet; the followed token **ID** remains saved, its art is refreshed after startup.
