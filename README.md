# SOS
===
Stephen's Operating System

Run full test suite on Unix:
$ make test

## Components
### Shell
- Test:
-- make test-shell-run
-- make test-shell-io-run

### Memory Manager
- Test: make test-memory-run

### Device Independent IO
- Test:
-- make test-io-run
-- make test-io-led-run
-- make test-io-button-run
-- make test-io-fs-run
-- make test-io-uart-run

- Supported Devices
-- /dev/led/orange
-- /dev/led/yellow
-- /dev/led/green
-- /dev/led/blue
-- /dev/button/sw1
-- /dev/button/sw2
-- /dev/uart/uart2
-- /dev/lcd/lcd
-- /dev/adc/potentiometer
-- /dev/adc/thermistor
-- /dev/touch/e1
-- /dev/touch/e2
-- /dev/touch/e3
-- /dev/touch/e4

### Supervisor Calls
- See man/svc.md

## Build Full System
To build and use SOS on the K70 in Code Warrior:
-- Create a new project in code warrior. UART IO or Debugger Console IO will both work.
-- Run ./create\_codewarrior\_import from sos/ to create a Sources directory in the parent directory
-- Import the Sources directory into the project
-- Import sos/freescaleK70/kinetis\_sysinit/kinetis_sysinit.c into Project\_Settings/Startup\_Code in the project.
-- Using Build Settings -> ARM Ltd Windows GCC C Compiler -> Preprocessor, create defined symbol K70
-- Click Debug and Play.
-- Connect via the UART using baud rate 115200. In Mac OSX, $ screen /dev/ttyX 115200 where ttyX is the Keyspan device.
-- Interact with the shell over the UART

## Hardware Demo Programs
These programs must be run on the K70. They are located in freescaleK70/
Follow the "Build Full System" instructions above,
and additionally add the appropriate preprocessor symbol for a given program (mutally exclusive):
- main\_button\_flash: BUTTON\_FLASH\_DEMO
- main\_device\_io: IO\_DEMO
- main\_lcd: LCD\_DEMO
- main\_potentiometer: POTENTIOMETER\_DEMO
- main\_svc: SVC\_DEMO
- main\_touch\_pads: TOUCH\_PAD\_DEMO
- main\_uart: UART\_DEMO
