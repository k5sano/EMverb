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

# Remove macOS quarantine attributes (Gatekeeper)
echo "Removing quarantine attributes..."
xattr -cr "$SCRIPT_DIR/EMVerb.vst3" 2>/dev/null || true
xattr -cr "$SCRIPT_DIR/EMVerb.component" 2>/dev/null || true
xattr -cr "$SCRIPT_DIR/EMVerb.app" 2>/dev/null || true

# VST3
if [ -d "$VST3_SRC" ]; then
    mkdir -p "$VST3_DST"
    rm -rf "$VST3_DST/EMVerb.vst3"
    cp -R "$VST3_SRC" "$VST3_DST/"
    xattr -cr "$VST3_DST/EMVerb.vst3" 2>/dev/null || true
    echo "[OK] VST3  -> $VST3_DST/EMVerb.vst3"
else
    echo "[SKIP] VST3 not found"
fi

# AU
if [ -d "$AU_SRC" ]; then
    mkdir -p "$AU_DST"
    rm -rf "$AU_DST/EMVerb.component"
    cp -R "$AU_SRC" "$AU_DST/"
    xattr -cr "$AU_DST/EMVerb.component" 2>/dev/null || true
    echo "[OK] AU    -> $AU_DST/EMVerb.component"
else
    echo "[SKIP] AU not found"
fi

# Standalone
if [ -d "$APP_SRC" ]; then
    cp -R "$APP_SRC" "$APP_DST/" 2>/dev/null || {
        echo "Installing Standalone to /Applications requires admin access..."
        sudo cp -R "$APP_SRC" "$APP_DST/"
    }
    xattr -cr "$APP_DST/EMVerb.app" 2>/dev/null || true
    echo "[OK] App   -> $APP_DST/EMVerb.app"
else
    echo "[SKIP] Standalone app not found"
fi

echo ""
echo "Done! Restart your DAW to detect the new plugin."
echo ""
echo "If macOS still blocks the plugin, go to:"
echo "  System Settings > Privacy & Security > Allow anyway"
