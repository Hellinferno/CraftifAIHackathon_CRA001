"""MJPEG HTTP Streaming Sink on :7010 for PipeGen v0.5.1 VMS Console."""

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
