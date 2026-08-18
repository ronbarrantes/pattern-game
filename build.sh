#!/usr/bin/env bash

set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
bin_dir="${project_dir}/bin"

mkdir -p "${bin_dir}"

gcc \
  -std=c11 \
  -Wall \
  -Wextra \
  -pedantic \
  -g \
  -fno-omit-frame-pointer \
  -fsanitize=address,undefined \
  "${project_dir}/pattern_game.c" \
  -o "${bin_dir}/pattern_game"

echo "Built ${bin_dir}/pattern_game"
