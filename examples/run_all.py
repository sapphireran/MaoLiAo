#!/usr/bin/env python3
"""Run every portable example test and demo. Exit non-zero on the first failure."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
REPO = ROOT.parent
PY = sys.executable

STEPS: list[tuple[str, list[str]]] = [
    ("physics tests", [PY, str(ROOT / "physics" / "test_inertia.py")]),
    ("jump demo", [PY, str(ROOT / "physics" / "demo_jump.py")]),
    ("run/stop demo", [PY, str(ROOT / "physics" / "demo_run_stop.py")]),
    ("collision tests", [PY, str(ROOT / "collision" / "test_aabb.py")]),
    ("overlap demo", [PY, str(ROOT / "collision" / "demo_overlap.py")]),
    ("input tests", [PY, str(ROOT / "input" / "test_commands.py")]),
    ("level tests", [PY, str(ROOT / "levels" / "test_worlds.py")]),
    ("level dump", [PY, str(ROOT / "levels" / "dump_layouts.py")]),
    ("level fixtures", [PY, str(ROOT / "levels" / "dump_layouts.py"), "--check-fixtures"]),
    ("save tests", [PY, str(ROOT / "save" / "test_save.py")]),
    ("tick tests", [PY, str(ROOT / "game_loop" / "test_tick.py")]),
    ("scripted run", [PY, str(ROOT / "game_loop" / "simulate_run.py")]),
]


def run(label: str, argv: list[str]) -> None:
    print(flush=True)
    print(f"==== {label} ====", flush=True)
    print(" ".join(argv), flush=True)
    completed = subprocess.run(argv, cwd=str(REPO), check=False)
    if completed.returncode != 0:
        raise SystemExit(f"{label} failed with {completed.returncode}")


def run_cpp() -> None:
    src = ROOT / "physics" / "inertia_port.cpp"
    bin_path = ROOT / "physics" / "inertia_port"
    compile_cmd = ["g++", "-std=c++17", "-O2", "-o", str(bin_path), str(src)]
    print(flush=True)
    print("==== compile C++ inertia port ====", flush=True)
    print(" ".join(compile_cmd), flush=True)
    compiled = subprocess.run(compile_cmd, cwd=str(REPO), check=False)
    if compiled.returncode != 0:
        print("g++ not available or compile failed; skipping C++ harness", flush=True)
        return
    run("C++ inertia port", [str(bin_path)])
    bin_path.unlink(missing_ok=True)


def main() -> int:
    print(f"python {sys.version.split()[0]}  repo {REPO}", flush=True)
    for label, argv in STEPS:
        run(label, argv)
    run_cpp()
    print()
    print("all portable examples passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
