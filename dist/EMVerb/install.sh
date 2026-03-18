#!/bin/bash
# EMVerb Installer — K5SANO
# Installs VST3, AU, and Standalone app to standard macOS locations.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

VST3_SRC="$SCRIPT_DIR/EMVerb.vst3"
AU_SRC="$SCRIPT_DIR/EMVerb.component"
APP_SRC="$SCRIPT_DIR/EMVerb.app"

VST3_DST="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DST="$HOME/Library/Audio/Plug-Ins/Components"
APP_DST="/Applications"

echo "==================================="
echo "  EMVerb Installer — K5SANO"
echo "==================================="
echo ""

# VST3
if [ -d "$VST3_SRC" ]; then
    mkdir -p "$VST3_DST"
    rm -rf "$VST3_DST/EMVerb.vst3"
    cp -R "$VST3_SRC" "$VST3_DST/"
    echo "[OK] VST3  -> $VST3_DST/EMVerb.vst3"
else
    echo "[SKIP] VST3 not found"
fi

# AU
if [ -d "$AU_SRC" ]; then
    mkdir -p "$AU_DST"
    rm -rf "$AU_DST/EMVerb.component"
    cp -R "$AU_SRC" "$AU_DST/"
    echo "[OK] AU    -> $AU_DST/EMVerb.component"
else
    echo "[SKIP] AU not found"
fi

# Standalone
if [ -d "$APP_SRC" ]; then
    cp -R "$APP_SRC" "$APP_DST/" 2>/dev/null || {
        # /Applications may require sudo
        echo "Installing Standalone to /Applications requires admin access..."
        sudo cp -R "$APP_SRC" "$APP_DST/"
    }
    echo "[OK] App   -> $APP_DST/EMVerb.app"
else
    echo "[SKIP] Standalone app not found"
fi

echo ""
echo "Done! Restart your DAW to detect the new plugin."
