#!/usr/bin/env bash

set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
bin_dir="${project_dir}/bin"

if ! command -v avr-gcc >/dev/null; then
  echo "Missing required command: avr-gcc" >&2
  exit 69
fi

mkdir -p "${bin_dir}"

avr-gcc \
  -std=gnu11 \
  -mmcu=attiny85 \
  -DF_CPU=1000000UL \
  -Os \
  -Wall \
  -Wextra \
  -Wpedantic \
  -ffunction-sections \
  -fdata-sections \
  -Wl,--gc-sections \
  "${project_dir}/pattern_game.c" \
  "${project_dir}/game.c" \
  "${project_dir}/panel.c" \
  "${project_dir}/sound.c" \
  "${project_dir}/timer.c" \
  -o "${bin_dir}/pattern_game.elf"

echo "Built ${bin_dir}/pattern_game.elf"
