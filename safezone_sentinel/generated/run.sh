#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

make
mkdir -p outputs models

if command -v deepstream-app >/dev/null 2>&1; then
    nohup deepstream-app -c deepstream_app_config.txt > outputs/pipeline_runtime.log 2>&1 &
    echo "[PipeGen v0.5.1] DeepStream pipeline started in background (PID $!)"
else
    python3 pipeline.py --config deepstream_app_config.txt
fi
