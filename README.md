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

### Supervisor Calls
- See README-SVC.md
- To demo:
-- Create a new project in code warrior with Debugger Console IO
-- Run ./create\_codewarrior\_import from sos/ to create a Sources directory in the parent directory
-- Import the Sources directory into the project
-- Import sos/freescaleK70/kinetis_sysinit.c into Project\_Settings/Startup\_Code in the project.
-- Using Build Settings -> ARM Ltd Windows GCC C Compiler -> Preprocessor, add defined symbols SOS and SVC_DEMO
-- Run the debugger (RAM). The console will show the output of sos/freescaleK70/main_svc.c

