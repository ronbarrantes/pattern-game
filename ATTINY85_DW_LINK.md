# Build an ATtiny85 programmer and debugWIRE probe

This guide starts with the programmer module already used by this project:

- Arduino Uno R4 Minima running ArduinoISP
- ATtiny85 RESET connected to Arduino D10
- MOSI connected to D11
- MISO connected to D12
- SCK connected to D13
- Shared 5 V and GND

That wiring is correct for flashing over ISP. The goal is to keep it working and
add a second mode for hardware debugging with
[dw-link](https://felias-fogg.github.io/dw-link/).

## The important limitation

The Uno R4 Minima can continue to flash the ATtiny85, but dw-link is made for an
AVR-based Uno R3. The R4 uses a Renesas RA4M1,
so do not plan on loading the dw-link firmware onto the R4.

The practical setup uses two Arduino boards at different times:

1. Uno R4 Minima for the existing ArduinoISP workflow.
2. A 5 V, 16 MHz Uno R3 with an ATmega328P for dw-link.

An ATmega328P Nano may be adaptable, but the current dw-link uploader and
quick-start explicitly document the Uno R3. Start with the verified board.
Once dw-link works, it can also program the ATtiny85, so the R4 will no longer
be required for this module.

## What the pieces do

- **ArduinoISP** writes firmware over SPI. It cannot pause or single-step the
  ATtiny85.
- **dw-link** switches between ISP programming and debugWIRE debugging.
- **avr-gdb** is the debugger running on the Mac.
- **The ELF file** contains the AVR program plus the source and symbol
  information that GDB needs.
- **debugWIRE** is the ATtiny85 debugging connection. It shares pin 1 with
  RESET.

## Parts

- Existing ATtiny85 socket module
- One Uno R3 based on the ATmega328P
- One 3-pin header and jumper, or one SPDT switch
- One 10 kΩ resistor
- One 100 nF ceramic capacitor
- One 10 µF electrolytic capacitor
- Optional status LED and 220 Ω resistor
- Headers, wire, and perfboard as needed

## ATtiny85 orientation

Check the notch or dot before applying power:

```text
             notch
          +---U---+
RESET/dW  1|     |8  VCC
PB3       2|     |7  PB2 / SCK
PB4       3|     |6  PB1 / MISO
GND       4|     |5  PB0 / MOSI
          +-------+
```

## Preserve the existing ISP wiring

These connections work in both modes:

| ATtiny85 | Signal | Arduino |
| --- | --- | --- |
| Pin 4 | GND | GND |
| Pin 5 | MOSI | D11 |
| Pin 6 | MISO | D12 |
| Pin 7 | SCK | D13 |
| Pin 8 | VCC | 5 V |

Your only mode-dependent connection is ATtiny85 pin 1:

| Mode | ATtiny85 pin 1 connects to |
| --- | --- |
| Uno R4 with ArduinoISP | D10 |
| Uno R3 with dw-link | D8 |

## Add a RESET mode selector

Install a 3-pin header in this order:

```text
[ D10 ] [ ATtiny RESET, pin 1 ] [ D8 ]
```

Place the jumper across:

- D10 and the center pin for Uno R4 ArduinoISP flashing.
- The center pin and D8 for Uno R3 dw-link.

Do not connect D8 and D10 together. Connect only one Arduino to the module at a
time.

An SPDT switch can replace the header. Connect its common terminal to ATtiny85
pin 1, then connect its two selectable terminals to D10 and D8.

## Add the support components

1. Connect 10 kΩ between ATtiny85 pin 1 and VCC.
2. Connect 100 nF directly between ATtiny85 pins 8 and 4.
3. On the Uno R3 used for dw-link, connect 10 µF between the Arduino
   RESET pin and GND. The positive lead goes to RESET and the negative lead goes
   to GND. This prevents USB serial auto-reset.
4. Do not place a capacitor from the ATtiny85 RESET pin to GND. That interferes
   with debugWIRE.

The optional dw-link status LED connects from D7 through 220 Ω to the LED
anode. Connect the LED cathode to GND.

## Test the module in its current flashing mode

Do this before adding debugWIRE software. It proves the socket, power, and SPI
wiring still work.

1. Put the selector in the D10 position.
2. Connect only the Uno R4.
3. Load ArduinoISP on the R4.
4. From this project, flash a small test:

   ```sh
   ./flash.sh practice/led_button.c
   ```

5. If automatic port detection finds the wrong device, set the port:

   ```sh
   ATTINY_PORT=/dev/cu.usbmodemXXXX ./flash.sh practice/led_button.c
   ```

Do not move on until this still works.

## Prepare the dw-link Arduino

Use a 5 V Uno R3 with an ATmega328P.

1. Download the uploader for the latest
   [dw-link release](https://github.com/felias-fogg/dw-link/releases), or clone
   the repository and upload its Arduino sketch.
2. Connect the Uno R3 to the Mac by USB.
3. Run the uploader for macOS.
4. Open a serial monitor at 115200 baud.
5. Send a single minus character, `-`.
6. A working dw-link should respond with `$#00`.

Follow the official
[dw-link AVR-GDB quick start](https://felias-fogg.github.io/dw-link/quickstart-AVR-GDB/)
if the uploader or serial check differs from the current release.

## Connect dw-link to the module

1. Disconnect the Uno R4 completely.
2. Put the selector in the D8 position.
3. Connect the Uno R3 running dw-link.
4. Confirm D11, D12, D13, 5 V, and GND are connected.
5. Confirm the ATtiny85 orientation.
6. Confirm there is only one power source.

The final dw-link wiring is:

| ATtiny85 | Uno R3 |
| --- | --- |
| Pin 1, RESET/debugWIRE | D8 |
| Pin 4, GND | GND |
| Pin 5, MOSI | D11 |
| Pin 6, MISO | D12 |
| Pin 7, SCK | D13 |
| Pin 8, VCC | 5 V |

## Install AVR GDB on the Mac

This project already uses the osx-cross AVR toolchain. Install its GDB package:

```sh
brew install avr-gdb
```

Verify it:

```sh
avr-gdb --version
```

## Build an ELF suitable for debugging

The project targets an ATtiny85 running at 1 MHz. A useful debug build needs
debug symbols and reduced optimization:

- Add `-g3`.
- Prefer `-Og` while debugging instead of `-Os`.
- Do not strip the ELF.

The current build output is:

```text
bin/pattern_game.elf
```

Keep the optimized release build separate if code size becomes tight.

## Prove command-line debugging before using Neovim

Start GDB with the project ELF:

```sh
avr-gdb -b 115200 bin/pattern_game.elf
```

At the GDB prompt, connect to the dw-link serial port:

```gdb
target remote /dev/cu.usbmodemXXXX
```

Find the exact device name with:

```sh
ls /dev/cu.usb*
```

Enable debugWIRE:

```gdb
monitor debugWIRE enable
```

GDB will ask you to power-cycle the target. Remove ATtiny power, wait a few
seconds, then restore it without disconnecting the debugger Arduino.

Load the ELF into the ATtiny:

```gdb
load
```

Try a small session:

```gdb
break main
continue
next
print sp
```

Once this works, the hardware, fuse state, ELF, serial connection, and AVR GDB
are all proven. Configure Neovim only after reaching this point.

## Exit debugWIRE safely

Before unplugging anything, run:

```gdb
monitor debugwire disable
quit
```

This matters. While debugWIRE is enabled, the RESET pin is unavailable for
normal reset and ISP programming. If GDB or the debugger crashes, reconnect and
exit cleanly before assuming the chip is damaged.

Do not change ATtiny fuses manually unless you have checked every fuse value. A
high-voltage programmer may be required to recover a chip after a bad fuse
configuration.

## Use the module as a programmer after debugging

dw-link can also program the ATtiny85 through the same D8, D11, D12, and D13
wiring. Disable debugWIRE first. The official tool exposes the programmer as
`Arduino as ISP`.

Keep the D10 selector position if you want to retain the current Uno R4 and
`flash.sh` workflow. Use the D8 position whenever the dw-link Arduino is
connected.

## Neovim comes last

The current Neovim CodeLLDB configuration is for native Mac executables. It
cannot debug an AVR ELF directly.

After the command-line GDB test works, add a separate ATtiny85 configuration
that starts `avr-gdb` and connects to dw-link. Keep the native CodeLLDB
configuration for ordinary C programs.

## Troubleshooting checklist

- Verify the ATtiny notch or pin-1 dot.
- Verify the selector position.
- Never connect the Uno R4 and dw-link Arduino at the same time.
- Verify one shared ground.
- Verify only one 5 V source is powering the module.
- Verify D11 to pin 5, D12 to pin 6, and D13 to pin 7.
- Verify the 10 kΩ RESET pull-up.
- Remove any capacitor from ATtiny RESET to GND.
- Close Arduino Serial Monitor before GDB opens the serial port.
- Verify the project clock is 1 MHz.
- Build with `-g3` before expecting source lines or variables.
- Run `monitor info` if dw-link reports a target connection error.
- Run `monitor debugwire disable` before returning to normal ISP flashing.

## References

- [dw-link AVR-GDB quick start](https://felias-fogg.github.io/dw-link/quickstart-AVR-GDB/)
- [dw-link manual](https://felias-fogg.github.io/dw-link/)
- [ATtiny25/45/85 datasheet](https://ww1.microchip.com/downloads/en/devicedoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf)
