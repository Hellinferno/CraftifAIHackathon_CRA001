"""SafeZone-RT Line-Crossing & Restricted Hazard Zone Counter for PipeGen v0.5.1."""

import time
from dataclasses import dataclass, field
from typing import Dict, List, Tuple


@dataclass
class ZoneCounterState:
    line_start: Tuple[int, int] = (320, 540)
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
        cy = top + height * 0.85

        prev = self.previous_centroids.get(tracker_id)
        self.previous_centroids[tracker_id] = (cx, cy)

        crossed_line = False
        line_y = self.line_start[1]
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
