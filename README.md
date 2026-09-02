# Pattern Game

This is some code for a simon says style game that I'll be building

## Flashing the ATtiny85

Connect an Arduino running the ArduinoISP sketch, then flash a C, ELF, or HEX file
with the same command on Linux and macOS:

```sh
./flash.sh practice/led_button.c
```

The script detects common Linux and macOS serial-port names. If more than one
serial device is connected, select the ArduinoISP explicitly:

```sh
ATTINY_PORT=/dev/cu.usbmodem1101 ./flash.sh practice/led_button.c
```

Pass every source file when flashing the full game:

```sh
./flash.sh pattern_game.c game.c panel.c sound.c timer.c
```
