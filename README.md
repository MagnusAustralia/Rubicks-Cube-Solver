# Rubik's Cube Solver (2x2 & 3x3)

A C++ application using **raylib** to visualize and solve 2x2 and 3x3 Rubik's Cubes interactively in 3D.

---

## Features

* **Interactive 3D Render:** Rotate, pan, and view the cube from any angle using raylib.
* **Multiple Cube Sizes:** Supports both 2x2 (Mini Cube) and 3x3 (Standard Cube) configurations.
* **Solving Algorithms:** Automatically calculates and steps through the solution.
* **Scramble Generator:** Generates valid, randomized scrambles.

---

## Prerequisites

* **C++ Compiler:** Supporting C++17 or later (GCC, Clang, or MSVC).
* **raylib:** Version 4.0 or higher.
* **CMake:** Version 3.14+ (if building via CMake).

---

## Installation & Setup

### 1. Clone the Repository
```bash
git clone https://github.com/your-username/rubiks-cube-solver.git
cd rubiks-cube-solver
```

### 2. Install Dependencies
Ensure `raylib` is installed on your system:

* **macOS (via Homebrew):**
  ```bash
  brew install raylib
  ```
* **Linux (Ubuntu/Debian):**
  ```bash
  sudo apt install libraylib-dev
  ```
* **Windows:**
  Follow the [raylib installation guide](https://github.com/raysan5/raylib) or use `vcpkg`:
  ```bash
  vcpkg install raylib
  ```

### 3. Build the Project
Using CMake:
```bash
mkdir build
cd build
cmake ..
make
```

---

## How to Run

After building, execute the binary from the `build` directory:

```bash
./rubiks_solver
```

### Keybinds & Controls

| Action | Control |
| :--- | :--- |
| **Rotate View** | Left Click + Drag |
| **Zoom In / Out** | Mouse Wheel |
| **Select Cube Type** | `1` (2x2) / `2` (3x3) |
| **Scramble Cube** | `S` |
| **Solve Cube** | `Spacebar` |

---

## Project Structure

```text
.
├── src/
│   ├── main.cpp          # Entry point and main render loop
│   ├── Cube.cpp          # Core cube state and rotation logic
│   ├── Solver.cpp        # Solving algorithms (2x2 and 3x3)
│   └── Renderer.cpp      # raylib 3D drawing functions
├── include/              # Header files
├── CMakeLists.txt        # Build configuration
└── README.md
```

---

## Future Improvements

* [ ] Add step-by-step move animation controls (Pause, Speed adjustment).
* [ ] Implement Kociemba's algorithm for optimal 3x3 solutions.
* [ ] Custom color themes for cube faces.