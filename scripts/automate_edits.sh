#!/usr/bin/env bash
set -euo pipefail

# -----------------------------------------------------------------------------
# scripts/automate_edits.sh
# Automate inline edits across all firmware files
# -----------------------------------------------------------------------------

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
echo "Project root: $ROOT"

# Detect sed-inplace differences
if [[ "$OSTYPE" == "darwin"* ]]; then
  SED_I=(-i '')
else
  SED_I=(-i)
fi

# Helper to insert FreeRTOS.h at top if missing
insert_freertos_header() {
  local file="$1"
  if ! grep -q '^#include "freertos/FreeRTOS.h"' "$file"; then
    # POSIX sed insert at line 1
    sed "${SED_I[@]}" '1i\
#include "freertos/FreeRTOS.h"
' "$file"
  fi
}

# 1) Strip any leading C-style comment blocks from these two
for F in "$ROOT/CMakeLists.txt" "$ROOT/sdkconfig.defaults"; do
  echo "→ Stripping header from $F"
  sed "${SED_I[@]}" '/^\/\*/,/\*\//d' "$F"
done

# 2) Fix include order in main.c
F="$ROOT/main/main.c"
echo "→ Fixing include order in $F"
insert_freertos_header "$F"

# 3) Wi-Fi manager tweaks
F="$ROOT/components/wifi_manager/wifi_manager.c"
echo "→ Editing $F"
# replace hard-coded creds with sdkconfig IDs
sed "${SED_I[@]}" 's/"YOUR_WIFI_SSID"/CONFIG_WIFI_SSID/g; s/"YOUR_WIFI_PASSWORD"/CONFIG_WIFI_PASSWORD/g' "$F"
insert_freertos_header "$F"
# prepend custom file header if missing
if ! grep -q "Version: 8.0.6" "$F"; then
  cat > "$F.tmp" <<EOF
/*
 * File: components/wifi_manager/wifi_manager.c
 *
 * Created on: 13 June 2025 10:00:00
 * Last edited on: 13 June 2025 12:40:00
 *
 * Version: 8.0.6
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */
EOF
  printf "\n" >> "$F.tmp"
  cat "$F" >> "$F.tmp"
  mv "$F.tmp" "$F"
fi

# 4) MQTT manager fixes
F="$ROOT/components/mqtt_manager/mqtt_manager.c"
echo "→ Editing $F"
# switch URI constant to sdkconfig
sed "${SED_I[@]}" 's|#define MQTT_BROKER_URI .*|#define MQTT_BROKER_URI CONFIG_MQTT_URI|' "$F"
insert_freertos_header "$F"

# 5) All other files (paths listed) — just echoing, no inline edits needed
for F in \
  "$ROOT/main/CMakeLists.txt" \
  "$ROOT/components/board_manager/include/board_manager.h" \
  "$ROOT/components/board_manager/board_manager.c" \
  "$ROOT/components/board_manager/CMakeLists.txt" \
  "$ROOT/components/app_logic/include/app_logic.h" \
  "$ROOT/components/app_logic/app_logic.c" \
  "$ROOT/components/app_logic/CMakeLists.txt" \
  "$ROOT/components/wifi_manager/CMakeLists.txt" \
  "$ROOT/components/mqtt_manager/include/mqtt_manager.h" \
  "$ROOT/components/mqtt_manager/CMakeLists.txt"
do
  echo "→ (no edits needed) $F"
done

echo "✅ All edits applied."
