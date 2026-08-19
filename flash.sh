#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: $0 FILE.c|FILE.elf|FILE.hex" >&2
  echo "Override the default port with ATTINY_PORT=/dev/ttyACM1" >&2
}

if [[ $# -ne 1 ]]; then
  usage
  exit 64
fi

input_file="$1"
if [[ ! -f "$input_file" ]]; then
  echo "File not found: $input_file" >&2
  exit 66
fi

if ! command -v avrdude >/dev/null; then
  echo "Missing required command: avrdude" >&2
  exit 69
fi

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
bin_dir="${project_dir}/bin"
port="${ATTINY_PORT:-/dev/ttyACM0}"
extension="${input_file##*.}"
extension="${extension,,}"

case "$extension" in
  c)
    for tool in avr-gcc avr-objcopy avr-size; do
      if ! command -v "$tool" >/dev/null; then
        echo "Missing required command: $tool" >&2
        exit 69
      fi
    done

    mkdir -p "$bin_dir"
    output_name="$(basename -- "${input_file%.*}")"
    elf_file="${bin_dir}/${output_name}.elf"
    hex_file="${bin_dir}/${output_name}.hex"

    avr-gcc \
      -std=gnu11 \
      -mmcu=attiny85 \
      -DF_CPU=1000000UL \
      -Os \
      -Wall \
      -Wextra \
      -ffunction-sections \
      -fdata-sections \
      -Wl,--gc-sections \
      "$input_file" \
      -o "$elf_file"

    avr-size --format=avr --mcu=attiny85 "$elf_file"
    avr-objcopy -O ihex -R .eeprom "$elf_file" "$hex_file"
    firmware_file="$hex_file"
    firmware_format="i"
    ;;
  elf)
    firmware_file="$input_file"
    firmware_format="e"
    ;;
  hex)
    firmware_file="$input_file"
    firmware_format="i"
    ;;
  *)
    echo "Unsupported file type: .$extension" >&2
    usage
    exit 65
    ;;
esac

if [[ ! -e "$port" ]]; then
  echo "ArduinoISP port not found: $port" >&2
  exit 69
fi

echo "Flashing $firmware_file to ATtiny85 via $port"
avrdude \
  -p t85 \
  -c stk500v1 \
  -P "$port" \
  -b 19200 \
  -U "flash:w:${firmware_file}:${firmware_format}"
