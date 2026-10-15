# PipeGen v0.5.1 Exact Installation, Cloud GPU & Execution Guide

**Extracted Directly From Your Uploaded Files:**

* `PipeGen_0.5.1_amd64.deb` (`Version: 0.5.1`, container `craftifaidocker/deepstream:7.1-amd64-v3`)

* `PipeGen_0.5.1_arm64.deb` (`Version: 0.5.1`, container `craftifaidocker/deepstream:7.1-arm64-v3` for Jetson)

* `pipegen_prerequisites_ubuntu_x86.sh`

* `PipeGen-v0.5.0-Installation-Guide.pdf`

## 1. What We Found Inside `PipeGen_0.5.1_amd64.deb` (Why Colab Won't Work & What Will)

Inspecting the `/opt/PipeGen/run` binary inside your uploaded `PipeGen_0.5.1_amd64.deb` reveals the **7 hard startup gates** that PipeGen enforces before it launches:

| Startup Check in `/opt/PipeGen/run` | Exact Requirement Enforced by `PipeGen_0.5.1_amd64.deb` | Runs on Google Colab? | Runs on Laptop (Ubuntu / Windows WSL2 with NVIDIA GPU) or [CraftifAI Cloud GPU](https://luma.com/5z8w42mq)? | 
| ----- | ----- | ----- | ----- | 
| **1. NVIDIA GPU & Driver** | `nvidia-smi` must report driver **`>= 560.35.03`** (`560.76+` on Windows WSL2 host) | **No** (Colab drivers vary and lack host display/Docker) | **Yes** | 
| **2. Docker Daemon** | `docker info` must succeed and pull `craftifaidocker/deepstream:7.1-amd64-v3` | **No** (Colab blocks nested `dockerd` with `--gpus all`) | **Yes** | 
| **3. NVIDIA Container Toolkit** | `nvidia-ctk` installed & configured for Docker runtime | **No** | **Yes** (Installed automatically by `pipegen_prerequisites_ubuntu_x86.sh`) | 
| **4. `socat` Utility** | `socat` installed to bridge host browser sign-in from the container | **Yes** | **Yes** | 
| **5. Display Session** | `$DISPLAY` set and `$XDG_SESSION_TYPE` must be **X11 / Xorg** (Wayland is explicitly blocked) | **No** (Headless notebook) | **Yes** (Xorg on Ubuntu, WSLg on Windows WSL2, or Cloud GPU X11 desktop) | 
| **6. Free Disk Space** | **`>= 38.0 GB` free** on `/var/lib/docker` (12 GB download + 35 GB unpacked DeepStream 7.1 image) | **No** | **Yes** (Pre-pulled on CraftifAI Cloud GPU!) | 
| **7. Directory Permissions** | `/opt/PipeGen` writable by `$USER` (`sudo chown -R $USER:$(id -gn) /opt/PipeGen`) | **N/A** | **Yes** | 

## 2. How to Install & Launch Your Uploaded `PipeGen_0.5.1_amd64.deb`

### Path A: On the CraftifAI Cloud GPU (or an Ubuntu 22.04 / 24.04 Laptop with an NVIDIA GPU)

Run these exact commands in the terminal where you uploaded `pipegen_prerequisites_ubuntu_x86.sh` and `PipeGen_0.5.1_amd64.deb`:

```
# 1. Run the official prerequisite installer (installs Docker, nvidia-ctk, socat, and docker group)
chmod +x ./pipegen_prerequisites_ubuntu_x86.sh
sudo ./pipegen_prerequisites_ubuntu_x86.sh -y

# 2. Install PipeGen v0.5.1 (.deb package)
sudo dpkg -i ./PipeGen_0.5.1_amd64.deb

# 3. Fix /opt/PipeGen ownership so the startup permission gate passes immediately
sudo chown -R $USER:$(id -gn) /opt/PipeGen
mkdir -p ~/.pipegen /home/$USER/videos

# 4. Activate the docker group in your current shell without rebooting
newgrp docker << 'EOF'
pipegen --version
pipegen
EOF

```

### Path B: On a Windows Laptop with an NVIDIA GPU (Using WSL2)

Per Page 6 of your `PipeGen-v0.5.0-Installation-Guide.pdf`, PipeGen runs on **Windows with WSL2 (Ubuntu)**:

1. Update your Windows NVIDIA Game Ready / Studio driver to **`560.76` or newer**.

2. Open **Ubuntu on WSL2** (`wsl --version` must be WSL 2, which includes WSLg X11 display support out of the box).

3. Ensure at least **38 GB of free disk space** is available inside WSL2 (`df -h /`).

4. Run the exact same 4 commands from **Path A** above inside your WSL2 Ubuntu terminal!

### Path C: If Your Laptop Has No NVIDIA GPU (or < 38 GB Free Disk Space)

Because the first launch of `PipeGen_0.5.1_amd64.deb` downloads the **12 GB (`35 GB` unpacked) `craftifaidocker/deepstream:7.1-amd64-v3` Docker image**, using the **CraftifAI Cloud GPU** provided on the [Hackathon Event Page](https://luma.com/5z8w42mq) is by far the fastest route:

* Message `#community-support` on Discord to claim a Cloud GPU session where `craftifaidocker/deepstream:7.1-amd64-v3` and `PipeGen v0.5.1` are already pre-installed.

## 3. What PipeGen v0.5.1 Actually Looks Like (Extracted from `/opt/PipeGen/frontend/assets/tutorial/`)

We extracted the 6 built-in tutorial screens from inside your `PipeGen_0.5.1_amd64.deb` package. Here is the exact workflow and file layout that `PipeGen v0.5.1` produces:

1. **Home Screen & Stage 1 Chat (`slide_0.png` & `slide_1.png`):**

   * Type your requirement into `"Mention your requirement..."`.

   * PipeGen replies with a 4-bullet summary (`Input`, `Detector`, `Tracker`, `Output`) and asks: *"Does that match what you had in mind?"*

   * **Important Tip from `slide_1.png`:** Ask PipeGen to include an **MJPEG stream on `:7010`** (`"Yes, and stream the output to the MJPEG server on :7010 plus JSON/MQTT logs"`) so the built-in **VMS Console (`127.0.0.1:7010`)** renders live video inside the PipeGen window!

2. **Stage 2 Pipeline IR Graph (`slide_2.png` — Mandatory Screenshot #1):**

   * Shows connected color-coded nodes with exact tensor shapes on the edges:

     * `INPUT` (`camera_0`, `1920x1080 @30fps`) → `uint8 [1, 1080, 1920, 3]`

     * `PREPROCESS` (`resize_norm`, `Letterbox to 640x640, scale to 0..1`) → `float32 [1, 3, 640, 640]`

     * `INFERENCE` (`detector`, `yolov8n.onnx`) → `float32 [1, 84, 8400]`

     * `POSTPROCESS` (`nms_decode`) → `float32 [1, 100, 6]`

     * `OUTPUT` (`display` / `mjpeg_sink`)

3. **Stage 3/4 `ONNX vs TRT` Accuracy & Speed Report (`slide_3.png` — Mandatory Screenshot #2):**

   * Displays the green **`✓ Report | ONNX vs TRT`** card with:

     * **`bbox_iou = 1.0`** (`mean IoU between matched ONNX & TRT boxes (1.0 = identical)`)

     * **Latency & FPS Speedup:** e.g., `ONNX 226.9484 ms · 4.4 FPS -> TRT 29.6218 ms · 33.8 FPS (▲ 7.7× faster)`

     * **Tensor Parity:** `cosine 1.0 · max|Δ| 1.2827`

4. **Stage 4/5 `✓ Compiled` 14-File Output Directory (`slide_4.png`):**

   * Generates **14 files** inside `generated/` and gives you the one-line launch command:

     ```
     cd /opt/pipegen/<project_name>/generated && ./run.sh
     
     ```

5. **Stage 5 Live VMS Console (`slide_5.png` — Mandatory Screenshot #3):**

   * Displays the live video stream (`video @ 127.0.0.1:7010 | mjpeg 640x480`) with green bounding boxes above the real-time **`DETECTIONS`** table (`TIME | SOURCE | LABEL | ID | SCORE`).