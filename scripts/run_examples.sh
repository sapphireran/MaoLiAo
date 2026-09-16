#!/usr/bin/env bash
# Build and run the portable MaoLiAo examples. Exit non-zero on a failed check.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root/examples"
make test
