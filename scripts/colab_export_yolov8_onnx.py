#!/usr/bin/env python3
"""Run in Google Colab (T4 GPU) to export a verified static [1, 3, 640, 640] YOLOv8 ONNX model for PipeGen Stage 3."""

from ultralytics import YOLO
import onnx


def export_and_verify() -> None:
    model = YOLO("yolov8n.pt")
    onnx_path = model.export(format="onnx", imgsz=640, opset=17, simplify=True, dynamic=False)
    print(f"[SafeZone-RT] Exported ONNX model: {onnx_path}")

    loaded = onnx.load(onnx_path)
    onnx.checker.check_model(loaded)
    for inp in loaded.graph.input:
        dims = [d.dim_value for d in inp.type.tensor_type.shape.dim]
        print(f"Verified Input Tensor: {inp.name} -> {dims}")
    for out in loaded.graph.output:
        dims = [d.dim_value for d in out.type.tensor_type.shape.dim]
        print(f"Verified Output Tensor: {out.name} -> {dims}")


if __name__ == "__main__":
    export_and_verify()
