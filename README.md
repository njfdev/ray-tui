<p align="center">
<img width="700" alt="Screenshot 2026-05-17 at 9 10 45 AM" src="https://github.com/user-attachments/assets/73d21632-162a-43df-9322-b69ab6fb2e01" />
</p>

# RayTUI
*What happens when two programmers, with nothing better to do, pair up for their Data Structures & Algorithms final? Well, RayTUI of course!*

RayTUI is ray-tracing within a terminal. That's it. We wanted to combine the challenge of building a ray-tracer from scratch and figuring out how to render in a terminal to depict 3D environments!

## Key Features
- BVHs
- ANSI escape sequences for terminal formatting
- Global input handling
- No 3rd-party dependencies

## User Guide

### Requirements
- RayTUI only works on macOS (tested with Tahoe 26.4.1) and Linux.
- A C++17 compiler (`gcc` or `clang`) and CMake
- A terminal emulator (any is fine, but we recommend [Ghostty](https://ghostty.org/download))

### Setup
1. Open your terminal emulator
2. Navigate to any folder
3. Run `git clone https://github.com/njfdev/ray-tui.git`
4. Run `cd ray-tui`
5. Run `./scripts/build.sh`

The demo applications should now be successfully compiled!

### Usage
From the `ray-tui` folder, all you have to do is run `./build/<demo-name>`. You can stop the demo by pressing `CTRL-C` or `CMD-C`.

Controls:
- WASD to move around
- R to move up, F to move down

The available demos are listed below.

**`./build/raytrace_demo` (The full demonstration of RayTUI's capabilities)**
<br/>
<img width="500" alt="Screenshot 2026-05-17 at 9 01 38 AM" src="https://github.com/user-attachments/assets/94d8d45f-9831-4e6e-bac9-2565e133e948" />

<br/>

**`./build/tui_gradient` (Shows terminal rendering ability)**
<br/>
<img width="500" alt="Screenshot 2026-05-17 at 9 05 41 AM" src="https://github.com/user-attachments/assets/d646cd99-22cf-458c-906d-e18e9e2d4c5b" />

<br/>

**`./build/raw_inputs` (Demo/test of global input handling)**
<br/>
<img width="500" alt="Screenshot 2026-05-17 at 9 08 26 AM" src="https://github.com/user-attachments/assets/9950ea28-dc6c-4b0c-8b00-93d1bfb11f31" />

