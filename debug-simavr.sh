#!/usr/bin/env bash

set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
firmware="${project_dir}/bin/pattern_game.elf"

if ! command -v simavr >/dev/null; then
  echo "Missing required command: simavr" >&2
  exit 69
fi

if [[ ! -f "${firmware}" ]]; then
  echo "Firmware not found. Run ./build.sh first." >&2
  exit 66
fi

echo "Starting simavr for ATtiny85 at 1 MHz"
echo "GDB will listen on localhost port 1234 until you stop this script."
exec simavr -g -m attiny85 -f 1000000 "${firmware}"
