# v0.3.2 — Internal RAM / Wi-Fi diagnostic fix

- Reduced LVGL partial draw buffer from 120 scanlines (57,600 bytes) to 30 scanlines (14,400 bytes), saving 43,200 bytes of internal RAM. Full-screen PSRAM canvas remains the same.
- Pixel HTTPS fetching disabled by default to verify Wi-Fi stability before enabling it.
- Settings exposes internal free RAM, largest allocatable block, PSRAM availability, and last Wi-Fi disconnect reason.
- Additional startup memory logging for TLS/Wi-Fi diagnosis.

# HoodOS v0.3.1 — Hotspot diagnostic update

- Disable STA Wi-Fi power saving (`WiFi.setSleep(false)`).
- Report network events and Wi-Fi disconnect reason.
- Defer HTTPS pixel request for 10 seconds after connection becomes stable.
- Log boot/reset reason and free heap around network requests.
- Include a one-line diagnostic switch (`HOODOS_ENABLE_PIXEL_FETCH`).
- No change to pixel artwork, LVGL display, swipe UI, apps or saved Hoodie IDs.

# Changelog

## v0.3 — 2026-10-24
- Connect to production `GET /v1/token/{id}/pixels` (20x20; 100 hex digits, MSB-first, 50 decoded bytes).
- Validate ID, dimensions, encoding, and reported black pixel count before displaying art.
- Draw real selected Hoodie 140x140 at 7x on home; use 120x120 at 6x on detail.
- Replace cramped six small icons with swipeable HOME/APPS/MORE pages and 2x2 large app tiles.
- Follow ID persists through ESP32 Preferences and triggers a new pixel fetch.
- Wi-Fi / HTTP status feedback, retry behavior and user-triggered refresh.
- Keep display hardware code from verified-working v0.2.1 firmware.

## v0.2.1
- RGB565 format fix and PSRAM allocation fallback.

## v0.2
- Modular apps, fonts, icons and followed Hoodie selection.
