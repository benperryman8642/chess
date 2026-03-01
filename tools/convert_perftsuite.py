#!/usr/bin/env python3
import re
import sys
from pathlib import Path

DEPTH_RE = re.compile(r"\bD(\d+)\s+(\d+)\b")

def main():
    if len(sys.argv) < 3:
        print("Usage: convert_perftsuite.py <perftsuite.epd> <out.txt> [max_depth]")
        sys.exit(2)

    epd_path = Path(sys.argv[1])
    out_path = Path(sys.argv[2])
    max_depth = int(sys.argv[3]) if len(sys.argv) >= 4 else 6

    if not epd_path.exists():
        print(f"Input file not found: {epd_path}")
        sys.exit(2)

    lines_out = []
    lines_out.append("# format: FEN ; depth ; nodes")
    lines_out.append("# generated from perftsuite.epd")

    with epd_path.open("r", encoding="utf-8", errors="replace") as f:
        for raw in f:
            raw = raw.strip()
            if not raw or raw.startswith("#"):
                continue

            # EPD line format: "<FEN> ;D1 <n> ;D2 <n> ;... ;"
            parts = [p.strip() for p in raw.split(";") if p.strip()]
            if not parts:
                continue

            fen = parts[0]
            # Basic sanity: FEN should have at least 4 fields
            if len(fen.split()) < 4:
                continue

            depth_map = {}
            for p in parts[1:]:
                m = DEPTH_RE.search(p)
                if m:
                    d = int(m.group(1))
                    n = int(m.group(2))
                    depth_map[d] = n

            for d in range(1, max_depth + 1):
                if d in depth_map:
                    lines_out.append(f"{fen} ; {d} ; {depth_map[d]}")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines_out) + "\n", encoding="utf-8")
    print(f"Wrote {out_path} ({len(lines_out)-2} tests)")

if __name__ == "__main__":
    main()