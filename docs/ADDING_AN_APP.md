# Adding a new app to HoodOS v0.3

The firmware uses small header-only modules and two-color direct canvas drawing.

1. Add a header in `firmware/HoodOS/apps/`, e.g. `RadioApp.h`, containing a `namespace RadioApp` with `inline void draw()` and `inline void tap(int x, int y)`.
2. Add the app to `HoodState::Screen` in `core/HoodState.h`.
3. Include its header in `core/HoodNavigation.h` and register the new enum value in both `render()` and `event()` touch routing.
4. Add a tile to `apps/AppsApp.h` and choose an existing icon from `assets/smallbits_icons.h`.
5. For a detail app use `HoodUI::header("RADIO")`, `HoodUI::bottomHome()`, and `HoodState::back()` on back taps. Opening apps with `HoodState::open(...)` remembers which launcher page to return to.
6. Access the selected token with `HoodState::followedId`. The current bitmap can be drawn using `HoodAPI::drawPortrait(x, y, pixelScale)` when `HoodAPI::ready(HoodState::followedId)`.

Keep network fetching out of app drawing callbacks. `HoodAPI::tick()` is invoked from the normal Arduino loop and handles retries, then rerenders the active screen when new art arrives. Canvas coordinates are 240x240, and native 8x8 icon patterns should be scaled by integer factors only.

**New routes:** Implement a new network service in `core/` and explicitly register its tick handler; never store private wallet keys or Bankr secrets on the device.
