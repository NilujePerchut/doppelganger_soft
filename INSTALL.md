# Installation

## toolchain install

```bash
cd archives
make
```

## bootloader compilation

```bash
cd bootloader
make clean && make
```

The resulting file is named Doppelganger_Bootloader_v0.1.0_SDCC_18f45k50_INTOSC.hex.
You can write it into the the PIC using any programmer you want.

Onced flashed, the Doppelganger will boot in bootloader mode (signaled by a
LED-R blinking at a fast rate). In this mode, the Doppelganger can be seen from
the PC (through USB) as USB device (VID=0x1807, PID=0x55AA).

## application compilation

```bash
cd application
make clean && make
```

The resulting file is named Doppelganger.hex and is available in the hex
directory.

## Installing udev rules for the Doppelganger bootloader

```bash
cp udev/Doppelganger.rules /etc/udev/rules.d/
```

Then reload the udev rules (as root):
```bash
udevadm control --reload-rules
```

## Flashing the Doppelganger firmware into the board

```bash
cd tools
./uploader8.py hex/Doppelganger.hex
```

## enjoy
