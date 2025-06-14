#!/usr/bin/env bash
set -e

echo "⮕ Moving unique .c/.h files into components/wifi_manager…"

# 1) Move any .c files that don't already exist
for src in components/esp32-wifi-manager/*.c; do
  file=`basename "$src"`
  dst="components/wifi_manager/$file"
  if [ -f "$src" ] && [ ! -f "$dst" ]; then
    git mv "$src" "$dst"
    echo "  Moved $file"
  else
    echo "  Skipped $file"
  fi
done

# 2) Move headers if that folder exists
if [ -d components/esp32-wifi-manager/include ]; then
  for src in components/esp32-wifi-manager/include/*; do
    file=`basename "$src"`
    dst="components/wifi_manager/include/$file"
    if [ ! -f "$dst" ]; then
      git mv "$src" "$dst"
      echo "  Moved include/$file"
    else
      echo "  Skipped include/$file"
    fi
  done
fi

# 3) Remove the now-empty legacy directory
git rm -r --ignore-unmatch components/esp32-wifi-manager

# 4) Commit the changes
git commit -m "chore: merge esp32-wifi-manager into wifi_manager and remove legacy dir"

echo "✅ Done."
