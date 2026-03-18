#!/bin/bash
# EMVerb Uninstaller — K5SANO

echo "==================================="
echo "  EMVerb Uninstaller — K5SANO"
echo "==================================="
echo ""

rm -rf "$HOME/Library/Audio/Plug-Ins/VST3/EMVerb.vst3" && echo "[OK] Removed VST3"
rm -rf "$HOME/Library/Audio/Plug-Ins/Components/EMVerb.component" && echo "[OK] Removed AU"

if [ -d "/Applications/EMVerb.app" ]; then
    rm -rf "/Applications/EMVerb.app" 2>/dev/null || sudo rm -rf "/Applications/EMVerb.app"
    echo "[OK] Removed Standalone app"
fi

# Remove user data (presets, settings)
read -p "Remove user presets and settings? (y/N): " answer
if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
    rm -rf "$HOME/Library/Application Support/EMVerb"
    echo "[OK] Removed user data"
fi

echo ""
echo "Done!"
