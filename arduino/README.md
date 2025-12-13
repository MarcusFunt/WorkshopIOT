# Arduino Sketches

This folder holds all workshop automation sketches.

## Structure

- Each project lives in its own folder under `arduino/`.
- The sketch filename must match the folder name so the Arduino IDE can open it correctly.
- Put reusable helpers in `arduino/libraries/` and include them with `#include "<file>.h"`.

## Suggested Workflow

1. Copy a template from `arduino/templates/` into a new folder named after your project.
2. Update the header comment to describe the hardware and wiring.
3. Use `const` pin mappings and helper functions for clarity.
4. Compile locally with `arduino-cli compile --fqbn <board-fqbn> <path-to-project>`.
5. Commit both code and wiring notes so others can reproduce your setup.
