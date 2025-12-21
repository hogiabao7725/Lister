#!/bin/bash
# Installation script for lister history wrapper
# This adds the lister wrapper function to your shell config

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WRAPPER_SCRIPT="$SCRIPT_DIR/lister_history.sh"
HISTORY_FILE="$SCRIPT_DIR/src/history.txt"

# Detect shell from $SHELL environment variable or check which rc file exists
if [ -n "$SHELL" ]; then
    case "$SHELL" in
        *zsh*)
            SHELL_RC="$HOME/.zshrc"
            ;;
        *bash*)
            SHELL_RC="$HOME/.bashrc"
            ;;
        *)
            # Fallback: check which rc file exists
            if [ -f "$HOME/.zshrc" ]; then
                SHELL_RC="$HOME/.zshrc"
            elif [ -f "$HOME/.bashrc" ]; then
                SHELL_RC="$HOME/.bashrc"
            else
                echo "Error: Could not detect shell. Please use bash or zsh."
                exit 1
            fi
            ;;
    esac
elif [ -f "$HOME/.zshrc" ]; then
    SHELL_RC="$HOME/.zshrc"
elif [ -f "$HOME/.bashrc" ]; then
    SHELL_RC="$HOME/.bashrc"
else
    echo "Error: Could not detect shell. Please use bash or zsh."
    exit 1
fi

# Check if already installed
if grep -q "lister_history.sh" "$SHELL_RC" 2>/dev/null; then
    echo "Lister history wrapper already installed in $SHELL_RC"
    echo "To reinstall, remove the line containing 'lister_history.sh' from $SHELL_RC"
    exit 0
fi

# Add source line to shell rc
echo "" >> "$SHELL_RC"
echo "# Lister history wrapper" >> "$SHELL_RC"
echo "export LISTER_HISTORY_DIR=\"$SCRIPT_DIR\"" >> "$SHELL_RC"
echo "source \"$WRAPPER_SCRIPT\"" >> "$SHELL_RC"

echo "Lister history wrapper installed successfully!"
echo "Added to: $SHELL_RC"
echo "History file: $HISTORY_FILE"
echo ""
echo "To use, restart your shell or run: source $SHELL_RC"
echo "Then use 'lister' command as usual - it will automatically log to history.txt"

