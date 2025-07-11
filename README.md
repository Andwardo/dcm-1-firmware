
feat: Implement provisioning captive portal and stable AP mode

This commit marks the successful implementation of the Wi-Fi provisioning feature. It introduces a robust, message-driven architecture that resolves the previous crash loops and results in a stable Access Point that serves a captive portal for configuration.

Key Architectural Changes:
- **Refactored to Message-Driven Design:** Replaced the simple Wi-Fi state model with a sophisticated state machine managed by a FreeRTOS task and message queue (`wifi_manager`). This decouples components and eliminates race conditions.
- **Introduced Central Controller:** The `main` function now acts as a central controller, orchestrating the application state based on events and NVS status.
- **Modularized HTTP Server:** The HTTP server and DNS logic for the captive portal are now fully encapsulated in the `http_app` component.

Bug Fixes and Stabilization:
- **Resolved `Invalid mbox` Crash:** The new architecture, where the main controller waits for the `WIFI_MANAGER_AP_STARTED_BIT` event before starting the web server, completely fixes the critical lwIP crash loop.
- **Corrected Build System Dependencies:** Fixed multiple build failures by correcting the `REQUIRES` vs. `PRIV_REQUIRES` scope in `CMakeLists.txt` and resolving all `#include` order issues.
- **Stabilized Scan API:** Implemented graceful error handling in the `/api/wifi-scan` endpoint to prevent device resets if a scan fails while in AP mode.

Current Status:
- The device boots cleanly and, when no credentials are in NVS, it successfully starts the "PianoGuard-Setup" Access Point.
- The captive portal is served at 192.168.4.1.
- The "Scan for Networks" API call is now stable and ready for final debugging.
