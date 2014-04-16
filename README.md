# SOS
===
Stephen's Operating System

Run full test suite:
$ make test

## Components
### Shell
- Build: make sh
- Run:   ./sh
- Test:  make test-shell-run

### Memory Manager
- Build: make map
- Run: ./map
- Test: make test-memory-run

### Device Independent IO
- Test: make test-io-run; make test-io-led-run; make test-io-button-run; make test-io-fs-run

## Freescale K70 Hardware Demos
### RotateLED
- Import Source files from freescaleK70/RotateLED and freescalek70/hardware into CodeWarrior

