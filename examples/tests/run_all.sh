#!/usr/bin/env bash
# Thin wrapper so docs can say "run the test script" as well as `make test`.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
make -C "$root" test
