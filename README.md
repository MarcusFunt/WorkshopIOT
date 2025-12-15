# Workshop IOT Arduino Collection

This repository hosts a growing collection of Arduino sketches for automating common workshop tasks. It is structured to make it easy to add, test, and share individual automation projects while keeping shared utilities organized.

## Repository Layout

- `arduino/` — Root folder for all sketches.
  - `templates/` — Starter sketches you can copy to begin a new automation idea.
  - `libraries/` — Optional shared headers or reusable components.
- `.gitignore` — Ignores common Arduino and PlatformIO build artifacts.

## Getting Started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) or the [arduino-cli](https://arduino.github.io/arduino-cli/latest/installation/) toolchain.
2. Connect your board and confirm it appears under **Tools → Port** (or via `arduino-cli board list`).
3. Copy a template from `arduino/templates/` into a new folder under `arduino/<project-name>/`, rename the sketch to match the folder, and start coding.

## Adding a New Sketch

1. Create a new folder under `arduino/` named after your project, e.g. `arduino/drill-press-light/`.
2. Place a `.ino` file inside the folder that uses the exact same name (e.g. `drill-press-light.ino`).
3. If you have shared code, add headers or source files to `arduino/libraries/` and include them in your sketch with `#include "<file>.h"`.
4. Document any wiring and component notes at the top of the sketch or add a `README.md` inside your project folder.

## Conventions

- Keep pin assignments and wiring diagrams in comments at the top of each sketch.
- Prefer `const` for pin numbers and configuration values.
- Use clear function names for actions (e.g. `enableVacuum()`, `toggleLights()`).
- Test with `arduino-cli compile --fqbn <board-fqbn> arduino/<project-name>` before flashing.

## Future Ideas

- Add CI that compiles sketches with `arduino-cli`.
- Include board-specific `arduino-cli.yaml` profiles to streamline local builds.

## Continuous Integration

This repository runs [`arduino-lint`](https://github.com/arduino/arduino-lint) in GitHub Actions to validate every sketch and shared library. The workflow installs the tool, caches its configuration in `~/.arduino-lint`, and runs `arduino-lint . --recursive --fail-on=warning` so warnings cause the check to fail.
