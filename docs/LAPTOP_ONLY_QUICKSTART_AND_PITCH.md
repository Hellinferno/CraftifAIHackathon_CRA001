# Laptop-Only 30-Minute Execution & Winning Pitch Guide (Track A: PipeGen)

**Event:** [CraftifAI Physical AI Hackathon](https://luma.com/5z8w42mq)  
**Official Problem Statement:** [Track A — PipeGen (Perception) + Edge Optimization Bonus](https://docs.google.com/document/d/1nxSV7QiLeO3DvOLIdt2AOiZQtMqIYqt5m2tSAqqwmAY/edit?usp=sharing)  
**Submission Form:** [PipeGen Track Submission Form](https://forms.gle/eJyeCKCG2UH757QZ8)

## 1. Why a Laptop-Only Entry Can Win Track A Outright

According to the [Official Problem Statement](https://docs.google.com/document/d/1nxSV7QiLeO3DvOLIdt2AOiZQtMqIYqt5m2tSAqqwmAY/edit?usp=sharing) and the [Luma Event Page](https://luma.com/5z8w42mq):
* **Tracks A and B are judged and awarded separately** ($250 cash + 5M CraftifAI Orbit credits each on the event page, contributing to the $1,000 prize pool). You do **not** need an ESP32 or STM32 board to win Track A.
* **What Track A requires:**
  1. **Full 5-Stage PipeGen v0.5.1 Flow:** Requirement → Pipeline IR → Model → Compiler → Run.
  2. **TensorRT Engine:** At least one ONNX model compiled to a TensorRT `.engine` with PipeGen's speed report.
  3. **Live Run:** Running live on a **local video file** (`/home/$USER/videos/...`) or your **laptop's built-in webcam** (`/dev/video0`).
  4. **Actionable Output:** Producing MQTT events, JSON logs, and real-time alerts beyond bounding boxes on a screen.
  5. **Edge Optimization Bonus (Track A):** Quantizing your model to **INT8** in PipeGen Stage 4 and reporting the speed, power, and accuracy trade-off against FP16/FP32.

## 2. Check Your Laptop GPU Setup (Local NVIDIA GPU vs. CraftifAI Cloud GPU)

Choose the path below that matches your laptop:

| Your Laptop Hardware | How to Run PipeGen v0.5.1 Today | Input Source to Use |
| :--- | :--- | :--- |
| **Laptop WITH an NVIDIA RTX / GTX GPU (Linux x86-64, X11 session)** | Download & install **PipeGen v0.5.1** directly from [developer.craftifai.com/dashboard?tab=downloads](https://developer.craftifai.com/dashboard?tab=downloads) and verify `pipegen --version` prints `0.5.1`. | Use either your **laptop built-in webcam (`/dev/video0`)** or a `.mp4` video placed under `/home/$USER/videos/`. |
| **Laptop WITHOUT an NVIDIA GPU (Windows / Mac / Integrated Graphics)** | As noted on the [Event Page](https://luma.com/5z8w42mq), CraftifAI provides **Cloud GPU access** for participants! Join the hackathon Discord (`#community-support`) immediately to claim a Cloud GPU session with PipeGen v0.5.1 pre-configured. | Use a **local `.mp4` safety video file** placed under `/home/$USER/videos/workplace_safety.mp4` on the Cloud GPU instance. |

## 3. 30-Minute Step-by-Step Checklist Before 11:00 PM IST

### Minute 0–5: Sign In & Prepare Video Input
1. Sign in at [developer.craftifai.com](https://developer.craftifai.com/) (activates 3,000,000 free credits automatically).
2. Confirm `pipegen --version` outputs `0.5.1`.
3. Place any 10–30 second workplace/construction/person video at `/home/$USER/videos/workplace_safety.mp4` (keep videos strictly under `/home` as required by the Setup Checklist in the [Official Problem Statement](https://docs.google.com/document/d/1nxSV7QiLeO3DvOLIdt2AOiZQtMqIYqt5m2tSAqqwmAY/edit?usp=sharing)), or use your laptop's built-in webcam (`/dev/video0`).

### Minute 5–15: Complete the 5 Stages in PipeGen v0.5.1
1. **Stage 1 (Requirement):** Paste the **Stage 1 Prompt** from `PIPEGEN_5_STAGE_BLUEPRINT.md`.
2. **Stage 2 (Pipeline IR):** Inspect the generated block graph (`Input → StreamMux → nvinfer → nvtracker → nvdsanalytics → OSD → MQTT/JSON Sinks`), click **Approve**, and **save Screenshot #1 (`pipeline_ir_node_graph.png`)**.
3. **Stage 3 (Model):** Select the YOLOv8 detection model from CraftifAI's benchmark catalog (or upload a YOLOv8 PPE ONNX model). Let PipeGen inspect the `[1, 3, 640, 640]` input tensor and generate the C++ output parser.
4. **Stage 4 (Compiler — Claim the Edge Optimization Bonus!):**
   * First compile with **FP16** precision and **save Screenshot #2A (`tensorrt_speed_report_fp16.png`)**.
   * Next compile with **INT8** precision and **save Screenshot #2B (`tensorrt_speed_report_int8.png`)**.
   * Copy the latency (ms), throughput (FPS), power draw, and accuracy agreement numbers from those two reports into the comparison table in `README_SUBMISSION_GUIDE.md`.
5. **Stage 5 (Run):** Run the generated pipeline (`./run.sh`), open the **PipeGen VMS Console**, and **save Screenshot #3 (`vms_live_detection_console.png`)** showing live bounding boxes and the running detection log.

### Minute 15–22: Launch the Laptop Actionable Dashboard
1. Save the `safezone_laptop_sentinel.py` script from `PIPEGEN_5_STAGE_BLUEPRINT.md` into your project folder.
2. Run it in a second terminal alongside PipeGen:
   ```bash
   python3 safezone_laptop_sentinel.py --http-port 8080
   ```
   *(Tip: To immediately test and screenshot the dashboard UI with sample alerts, run `python3 safezone_laptop_sentinel.py --simulate-demo` and open `http://localhost:8080` in your laptop browser.)*
3. **Save Screenshot #4 (`actionable_dashboard_alerts.png`)** showing the red `HAZARD_ALERT` banner, violation counters, and live incident table.

### Minute 22–30: Push to GitHub & Submit Form
1. Create a public GitHub repository named `safezone-rt-pipegen`.
2. Add your exported PipeGen directory (`pipegen_export/`), `safezone_laptop_sentinel.py`, your 4 evidence screenshots (`docs/evidence/`), and `README_SUBMISSION_GUIDE.md` (renamed to `README.md`).
3. Submit your repository link, project description, and screenshots via the official **PipeGen Track Submission Form**: [https://forms.gle/eJyeCKCG2UH757QZ8](https://forms.gle/eJyeCKCG2UH757QZ8).

## 4. Winning 2-Minute Live Demo Script for Judges

When presenting your laptop screen to the judges, follow this crisp 4-part flow:

1. **The Problem & Why PipeGen Is Load-Bearing (20 sec):**
   > "Industrial CCTV cameras passively record workplace accidents instead of preventing them, and general-purpose AI scripts fail when porting ONNX models to DeepStream due to tensor parser mismatches and CPU decode bottlenecks. We built **SafeZone-RT** end-to-end inside **PipeGen v0.5.1** to detect PPE violations and restricted hazard-zone intrusions in real time."
2. **Show Stage 2 Pipeline IR & Stage 3 Structural Model Check (30 sec):**
   > "Here is our hardware-neutral **Stage 2 Pipeline IR** graph combining YOLOv8 detection, NvDCF multi-object tracking, and polygon ROI hazard analytics. In **Stage 3**, PipeGen inspected our model's `[1, 9, 8400]` output tensor directly and compiled the custom C++ bounding-box parser (`NvDsInferParseCustomYoloV8`) automatically."
3. **Show Stage 4 INT8 Edge Optimization Bonus (30 sec):**
   > "For the **Edge Optimization Bonus**, we compiled the model into both **FP16** and **INT8** TensorRT engines in Stage 4. Here is PipeGen's benchmark report showing our INT8 engine throughput, tail latency, and accuracy agreement verified directly against the source ONNX model."
4. **Show Stage 5 Live VMS + Actionable Laptop Command Center (40 sec):**
   > "Finally, running `./run.sh` launches the pipeline live in the **VMS Console**. Rather than just drawing boxes on a screen, PipeGen streams every violation over MQTT and JSON to our **SafeZone-RT Actionable Alert Service** (`localhost:8080`), which filters out single-frame noise using 3-frame persistence, triggers immediate desktop/audio alarms on the laptop, and logs auditable safety incidents in real time."
