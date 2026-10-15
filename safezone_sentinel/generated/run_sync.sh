#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

make
mkdir -p outputs models

if command -v deepstream-app >/dev/null 2>&1; then
    exec deepstream-app -c deepstream_app_config.txt
else
    exec python3 pipeline.py --config deepstream_app_config.txt
fi
