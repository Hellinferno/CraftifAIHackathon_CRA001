# PipeGen v0.5.1 Five-Stage Blueprint & 14-File `generated/` Reference

**Extracted from `PipeGen_0.5.1_amd64.deb` (`craftifaidocker/deepstream:7.1-amd64-v3`)**  
**Project:** SafeZone-RT — Real-Time Workplace PPE & Hazard Zone Sentinel  
**Official Problem Statement:** [Track A — PipeGen (Perception) + Edge Optimization Bonus](https://docs.google.com/document/d/1nxSV7QiLeO3DvOLIdt2AOiZQtMqIYqt5m2tSAqqwmAY/edit?usp=sharing)

## Stage 1: Exact Chat Prompts for PipeGen v0.5.1 (`slide_0.png` & `slide_1.png`)

When you launch `pipegen` (`PipeGen v0.5.1`), it opens the chat screen (`"Ready when you are — Describe your Edge AI pipeline requirements and I'll help you design, compile, and run it."`).

### Prompt 1 (Initial Requirement — Copy-Paste into `"Mention your requirement..."`)

> I need a real-time workplace safety and PPE compliance pipeline (`safezone_sentinel`) running on `/home/$USER/videos/workplace_safety.mp4` at 1920x1080 30 FPS. Use YOLOv8 object detection compiled to TensorRT (both FP16 and INT8 for edge optimization) to detect people and PPE compliance (`person`, `hardhat`, `no-hardhat`, `safety-vest`, `no-safety-vest`), NvDCF multi-object tracking so each worker keeps a unique ID, and a line-crossing / ROI hazard zone counter. Output an MJPEG stream on `:7010` for the VMS Console plus JSON and MQTT event logs (`craftifai/safezone/alerts`).

### Prompt 2 (Confirmation Reply When PipeGen Summarizes the Plan)

When PipeGen replies with `"Got it. Here's what I'll build... Does that match what you had in mind?"`, reply:

> Yes, make sure the sink includes the MJPEG server on `:7010` for the VMS Console along with the line-crossing/zone counter and `msgconv_config.txt` metadata output. Generate the Pipeline IR now.

## Stage 2: Pipeline IR Block Graph (`slide_2.png` — Screenshot #1)

Verify that PipeGen generates the 5-stage hardware-neutral Pipeline IR graph with tensor dimensions displayed on every connecting edge, and **capture Screenshot #1 (`pipeline_ir_node_graph.png`)**:

| Pipeline IR Block | Node Name & Subtype | Output Edge Tensor Shape | Description in PipeGen v0.5.1 UI |
| :--- | :--- | :--- | :--- |
| **1. INPUT** | `camera_0` (`file` / `RTSP` / `v4l2`) | `uint8 [1, 1080, 1920, 3]` | Video stream at `1920x1080 @30fps` |
| **2. PREPROCESS** | `resize_norm` | `float32 [1, 3, 640, 640]` | `Letterbox to 640x640, scale to 0..1` |
| **3. INFERENCE** | `detector` (`yolov8n.onnx` / `yolov8s_ppe.onnx`) | `float32 [1, 84, 8400]` (or `[1, 9, 8400]`) | `YOLOv8 object detector` compiled to TensorRT |
| **4. POSTPROCESS** | `nms_decode` + `NvDCF` tracker | `float32 [1, 100, 6]` | `NMS decode` + persistent object IDs |
| **5. OUTPUT** | `mjpeg_sink` (`:7010`) + `msgconv` | `mjpeg 640x480` + JSON/MQTT events | Live VMS Console stream on `127.0.0.1:7010` + actionable event logs |

## Stage 3 & Stage 4: `ONNX vs TRT` Accuracy & Speed Report (`slide_3.png` — Screenshot #2)

When PipeGen compiles your ONNX model into a TensorRT engine inside the `craftifaidocker/deepstream:7.1-amd64-v3` container, it produces the **`✓ Report | ONNX vs TRT`** card.

To claim the **Edge Optimization Bonus (Track A)**, run the compiler for both **FP16** and **INT8**, **capture Screenshot #2 (`tensorrt_speed_report.png`)**, and record the exact metrics from PipeGen's report card in the table below:

| PipeGen Report Metric (`ONNX vs TRT` Card) | PipeGen Built-In Tutorial Reference (`slide_3.png`) | Your Run: Source ONNX (FP32) | Your Run: TensorRT FP16 | Your Run: TensorRT INT8 (Edge Optimization Bonus) |
| :--- | :--- | :--- | :--- | :--- |
| **`bbox_iou` (Mean IoU ONNX vs TRT, `1.0 = identical`)** | `1.0` | `1.0 (Reference)` | `<Record bbox_iou>` | `<Record bbox_iou>` |
| **Inference Latency (`ms`)** | `ONNX 226.9484 ms -> TRT 29.6218 ms` | `<Record ONNX ms>` | `<Record FP16 ms>` | `<Record INT8 ms>` |
| **Throughput (`FPS`)** | `ONNX 4.4 FPS -> TRT 33.8 FPS` | `<Record ONNX FPS>` | `<Record FP16 FPS>` | `<Record INT8 FPS>` |
| **Speedup Multiplier (`▲ X× faster`)** | `▲ 7.7× faster` (Detection) / `▲ 9.2× faster` (Pipeline) | `1.0×` | `<Record ▲ X× faster>` | `<Record ▲ X× faster>` |
| **Tensor Parity (`cosine` / `max\|Δ\|`)** | `cosine 1.0 · max\|Δ\| 1.2827` | `cosine 1.0` | `<Record cosine & max\|Δ\|>` | `<Record cosine & max\|Δ\|>` |

## Stage 5: The Exact 14 Files in `generated/` (`slide_4.png`)

As shown in `slide_4.png` inside `PipeGen_0.5.1_amd64.deb`, when compilation succeeds (`✓ Compiled | 14 files · generated/`), PipeGen outputs these **14 exact files** in `/opt/pipegen/<project_name>/generated/` (which you must push to your GitHub submission repo!):

| # | Generated File Path | Purpose in the DeepStream 7.1 Container |
| :--- | :--- | :--- |
| **1** | `config_infer_primary_yolov8n.txt` | Primary `nvinfer` TensorRT engine, ONNX path, and custom bbox parser config |
| **2** | `config_tracker_nvdcf.yml` | NVIDIA `NvDCF` multi-object tracker configuration |
| **3** | `deepstream_app_config.txt` | Top-level DeepStream pipeline configuration |
| **4** | `labels.txt` | Detection class labels (`person`, `hardhat`, `no-hardhat`, `safety-vest`, `no-safety-vest`) |
| **5** | `nvdsinfer_custom_impl/nvdsparsebbox_yolov8.cpp` | Custom C++ YOLOv8 bounding-box tensor parser for `nvinfer` |
| **6** | `nvdsinfer_custom_impl/Makefile` | Builds `libnvdsinfer_custom_impl_YoloV8.so` against DeepStream 7.1 & CUDA |
| **7** | `pipeline.py` | GStreamer / DeepStream Python orchestration script |
| **8** | `line_crossing_counter.py` | Spatial ROI hazard-zone and line-crossing analytics module |
| **9** | `mjpeg_sink.py` | Streams annotated frames to `127.0.0.1:7010` for the PipeGen VMS Console |
| **10** | `msgconv_config.txt` | `nvmsgconv` schema configuration for JSON and MQTT actionable event payloads |
| **11** | `requirements.txt` | Python dependencies inside the container |
| **12** | `Makefile` | Top-level build file compiling the custom C++ parser before pipeline start |
| **13** | `run.sh` | Asynchronous pipeline launcher used by the VMS Console |
| **14** | `run_sync.sh` | Synchronous foreground pipeline launcher for terminal debugging |

### Reference Implementation of the Key Generated Files (For Your GitHub Repo)

Below are clean, ready-to-use implementations of the key files in `generated/` matching PipeGen v0.5.1's exact naming conventions:

#### 1. `nvdsinfer_custom_impl/nvdsparsebbox_yolov8.cpp`

```cpp
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "nvdsinfer_custom_impl.h"

extern "C" bool NvDsInferParseCustomYoloV8(
    std::vector<NvDsInferLayerInfo> const& outputLayersInfo,
    NvDsInferNetworkInfo const& networkInfo,
    NvDsInferParseDetectionParams const& detectionParams,
    std::vector<NvDsInferObjectDetectionInfo>& objectList)
{
    if (outputLayersInfo.empty()) return false;
    const NvDsInferLayerInfo& layer = outputLayersInfo[0];
    const float* data = static_cast<const float*>(layer.buffer);

    const unsigned int numClasses = detectionParams.numClassesConfigured;
    const unsigned int channels = layer.inferDims.d[0];
    const unsigned int numAnchors = layer.inferDims.d[1];
    if (channels < 4 + numClasses) return false;

    for (unsigned int i = 0; i < numAnchors; ++i) {
        float maxProb = 0.0f;
        int maxClassId = -1;
        for (unsigned int c = 0; c < numClasses; ++c) {
            float prob = data[(4 + c) * numAnchors + i];
            if (prob > maxProb) {
                maxProb = prob;
                maxClassId = static_cast<int>(c);
            }
        }
        if (maxClassId < 0 || maxProb < detectionParams.perClassPreclusterThreshold[maxClassId]) continue;

        float cx = data[0 * numAnchors + i];
        float cy = data[1 * numAnchors + i];
        float w  = data[2 * numAnchors + i];
        float h  = data[3 * numAnchors + i];

        float left   = std::max(0.0f, cx - w * 0.5f);
        float top    = std::max(0.0f, cy - h * 0.5f);
        float right  = std::min(static_cast<float>(networkInfo.width), cx + w * 0.5f);
        float bottom = std::min(static_cast<float>(networkInfo.height), cy + h * 0.5f);
        if (right <= left || bottom <= top) continue;

        NvDsInferObjectDetectionInfo obj;
        obj.classId = maxClassId;
        obj.left = left;
        obj.top = top;
        obj.width = right - left;
        obj.height = bottom - top;
        obj.detectionConfidence = maxProb;
        objectList.push_back(obj);
    }
    return true;
}

CHECK_CUSTOM_PARSE_FUNC_PROTOTYPE(NvDsInferParseCustomYoloV8);
```

#### 2. `line_crossing_counter.py` (Hazard Zone & Line-Crossing Analytics Module)

```python
"""SafeZone-RT Line-Crossing & Restricted Hazard Zone Counter for PipeGen v0.5.1."""

import json
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Tuple


@dataclass
class ZoneCounterState:
    line_Start: Tuple[int, int] = (320, 540)
    line_end: Tuple[int, int] = (1600, 540)
    restricted_polygon: List[Tuple[int, int]] = field(
        default_factory=lambda: [(400, 280), (1520, 280), (1680, 980), (260, 980)]
    )
    previous_centroids: Dict[int, Tuple[float, float]] = field(default_factory=dict)
    counted_ids: set = field(default_factory=set)
    total_crossings: int = 0
    zone_violations: int = 0

    @staticmethod
    def point_in_polygon(x: float, y: float, poly: List[Tuple[int, int]]) -> bool:
        inside = False
        n = len(poly)
        px1, py1 = poly[0]
        for i in range(1, n + 1):
            px2, py2 = poly[i % n]
            if y > min(py1, py2) and y <= max(py1, py2) and x <= max(px1, px2):
                if py1 != py2:
                    xinters = (y - py1) * (px2 - px1) / (py2 - py1) + px1
                if px1 == px2 or x <= xinters:
                    inside = not inside
            px1, py1 = px2, py2
        return inside

    def update(self, tracker_id: int, label: str, confidence: float, bbox: Tuple[float, float, float, float]) -> dict | None:
        left, top, width, height = bbox
        cx = left + width * 0.5
        cy = top + height * 0.85  # Footpoint for accurate floor zone checking

        prev = self.previous_centroids.get(tracker_id)
        self.previous_centroids[tracker_id] = (cx, cy)

        crossed_line = False
        line_y = self.line_Start[1]
        if prev is not None and tracker_id not in self.counted_ids:
            if (prev[1] < line_y <= cy) or (prev[1] > line_y >= cy):
                self.counted_ids.add(tracker_id)
                self.total_crossings += 1
                crossed_line = True

        in_hazard_zone = self.point_in_polygon(cx, cy, self.restricted_polygon)
        is_ppe_violation = label in {"no-hardhat", "no-safety-vest"}

        if in_hazard_zone or is_ppe_violation or crossed_line:
            if in_hazard_zone or is_ppe_violation:
                self.zone_violations += 1
            return {
                "timestamp": time.strftime("%H:%M:%S"),
                "source": "cam0",
                "tracker_id": tracker_id,
                "label": label,
                "score": round(confidence * 100, 1),
                "in_restricted_zone": in_hazard_zone,
                "crossed_boundary": crossed_line,
                "total_crossings": self.total_crossings,
                "total_violations": self.zone_violations,
            }
        return None
```

#### 3. `mjpeg_sink.py` (VMS Console MJPEG Stream Server on `127.0.0.1:7010`)

```python
"""MJPEG HTTP Streaming Sink on :7010 for PipeGen v0.5.1 VMS Console (slide_1.png & slide_5.png)."""

import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer


class MJPEGFrameBuffer:
    def __init__(self) -> None:
        self.lock = threading.Condition()
        self.latest_jpeg: bytes = b""

    def update_frame(self, jpeg_bytes: bytes) -> None:
        with self.lock:
            self.latest_jpeg = jpeg_bytes
            self.lock.notify_all()


FRAME_BUFFER = MJPEGFrameBuffer()


class MJPEGHandler(BaseHTTPRequestHandler):
    def do_GET(self) -> None:
        self.send_response(200)
        self.send_header("Age", "0")
        self.send_header("Cache-Control", "no-cache, private")
        self.send_header("Pragma", "no-cache")
        self.send_header("Content-Type", "multipart/x-mixed-replace; boundary=FRAME")
        self.end_headers()
        try:
            while True:
                with FRAME_BUFFER.lock:
                    FRAME_BUFFER.lock.wait(timeout=1.0)
                    frame = FRAME_BUFFER.latest_jpeg
                if not frame:
                    time.sleep(0.03)
                    continue
                self.wfile.write(b"--FRAME\r\n")
                self.send_header("Content-Type", "image/jpeg")
                self.send_header("Content-Length", str(len(frame)))
                self.end_headers()
                self.wfile.write(frame)
                self.wfile.write(b"\r\n")
        except (BrokenPipeError, ConnectionResetError):
            pass

    def log_message(self, fmt: str, *args) -> None:
        return


def start_mjpeg_server(host: str = "127.0.0.1", port: int = 7010) -> ThreadingHTTPServer:
    server = ThreadingHTTPServer((host, port), MJPEGHandler)
    threading.Thread(target=server.serve_forever, daemon=True).start()
    print(f"[PipeGen MJPEG Sink] Streaming live video to http://{host}:{port} for VMS Console")
    return server
```
