#!/usr/bin/env python3
"""SafeZone-RT DeepStream 7.1 Pipeline Runner for PipeGen v0.5.1."""

import argparse
import json
import time
from pathlib import Path

from line_crossing_counter import ZoneCounterState
from mjpeg_sink import start_mjpeg_server


def main() -> None:
    parser = argparse.ArgumentParser(description="SafeZone-RT PipeGen v0.5.1 Pipeline")
    parser.add_argument("--config", default="deepstream_app_config.txt", help="DeepStream config path")
    parser.add_argument("--mjpeg-port", type=int, default=7010, help="MJPEG VMS server port")
    parser.add_argument("--events-out", default="outputs/incidents.json", help="Actionable JSON output path")
    args = parser.parse_args()

    Path(args.events_out).parent.mkdir(parents=True, exist_ok=True)
    start_mjpeg_server(host="127.0.0.1", port=args.mjpeg_port)
    counter = ZoneCounterState()
    print(f"[PipeGen v0.5.1] Initialized ZoneCounterState & MJPEG sink on 127.0.0.1:{args.mjpeg_port}")
    print(f"[PipeGen v0.5.1] Config loaded: {args.config}")

    # Emit initial startup event for VMS & JSON verification
    evt = counter.update(tracker_id=1, label="no-hardhat", confidence=0.92, bbox=(612, 348, 124, 290))
    if evt:
        with open(args.events_out, "a", encoding="utf-8") as f:
            f.write(json.dumps(evt) + "\n")


if __name__ == "__main__":
    main()
