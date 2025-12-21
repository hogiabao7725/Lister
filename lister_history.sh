#!/bin/bash
# Wrapper script to record lister command history
# Usage: source this file or add to your shell config

# Use LISTER_HISTORY_DIR if set, otherwise try to detect script directory
if [ -n "${LISTER_HISTORY_DIR}" ]; then
    SCRIPT_DIR="$LISTER_HISTORY_DIR"
elif [ -n "${BASH_SOURCE[0]}" ]; then
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
else
    # Fallback for zsh or other shells
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
fi

# Path to lister executable (can be overridden)
LISTER_BIN="${LISTER_BIN:-${SCRIPT_DIR}/bin/lister}"

# History file path (can be overridden)
HISTORY_FILE="${LISTER_HISTORY_FILE:-${SCRIPT_DIR}/src/history.txt}"

# Check if lister binary exists
if [ ! -x "$LISTER_BIN" ]; then
    echo "Warning: lister binary not found at $LISTER_BIN" >&2
    echo "Set LISTER_BIN environment variable to specify correct path" >&2
    return 1 2>/dev/null || exit 1
fi

# Wrapper function to log command and output
lister() {
    # Build command string for logging
    local cmd="lister"
    local arg
    for arg in "$@"; do
        # Quote argument if it contains spaces or special characters
        if [[ "$arg" =~ [[:space:]$'\n'\"\'\`] ]]; then
            # Escape quotes and wrap in double quotes
            local escaped_arg="${arg//\"/\\\"}"
            cmd+=" \"$escaped_arg\""
        else
            cmd+=" $arg"
        fi
    done

    # Get timestamp
    local timestamp
    timestamp=$(date '+%Y-%m-%d %H:%M:%S' 2>/dev/null || date '+%Y-%m-%d %H:%M:%S')

    # Capture output and exit code
    local output
    local exit_code
    output=$("$LISTER_BIN" "$@" 2>&1)
    exit_code=$?

    # Write to history file
    {
        echo "=== [$timestamp] ==="
        echo "Command: $cmd"
        echo "Exit Code: $exit_code"
        echo "Result:"
        echo "$output"
        echo ""
    } >> "$HISTORY_FILE" 2>/dev/null || {
        echo "Warning: Could not write to history file: $HISTORY_FILE" >&2
    }

    # Print output to user
    echo "$output"

    return $exit_code
}

