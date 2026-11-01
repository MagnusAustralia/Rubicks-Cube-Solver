# Rubik's Cube Solver (2x2 & 3x3)

A C++17 project that models, scrambles and solves 2x2 and 3x3 Rubik's Cubes. It includes a command-line menu, an optional graphical front end built with [SplashKit](https://splashkit.io/), and implementations of two classic solving approaches: **Korf's algorithm** and **Kociemba's algorithm**, backed by pattern databases indexed with **Lehmer codes**.

---

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Make Targets](#make-targets)
- [Pattern Databases](#pattern-databases)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Development](#development)
- [Troubleshooting](#troubleshooting)
- [Roadmap](#roadmap)
- [Contributing](#contributing)

---

## Features

- **2x2 and 3x3 cube models** with full move support.
- **Scramble generator** for producing random, valid cube states.
- **Two solvers for the 3x3:**
  - **Korf's algorithm**: iterative-deepening search guided by pattern databases.
  - **Kociemba's two-phase algorithm**: reduces the cube to a subgroup, then solves it.
- **Depth-first search** module used as a building block for the solvers.
- **Lehmer code** utilities for compactly ranking and indexing permutations.
- **Pattern database generator** for corners, edges and slice-based tables.
- **Two interfaces:** a CLI menu (default) and a SplashKit GUI.
- **Developer tooling:** `clang-format` and `clang-tidy` targets built into the Makefile.

---

## Requirements

| Requirement | Needed for | Notes |
|---|---|---|
| `clang++` with C++17 support | Everything | The Makefile uses `clang++` by default |
| `make` | Everything | Build and run targets live in the Makefile |
| [SplashKit](https://splashkit.io/) (`skm`) | GUI only | The `gui` target compiles with `skm clang++` |
| `clang-format`, `clang-tidy` | Linting only | Optional |

The CLI and all solver targets do **not** require SplashKit. Only the GUI does.

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/MagnusAustralia/Rubicks-Cube-Solver.git
cd Rubicks-Cube-Solver
```

### 2. Install a compiler

- **macOS:** `xcode-select --install`
- **Ubuntu/Debian:** `sudo apt install clang make`
- **Windows:** use WSL, or install LLVM/Clang with a Make-compatible environment (e.g. MSYS2).

### 3. (Optional) Install SplashKit for the GUI

Follow the [SplashKit installation guide](https://splashkit.io/installation/) for your platform and confirm `skm` is on your `PATH`:

```bash
skm --help
```

### 4. Build and run

```bash
make
```

The default target builds and launches the CLI menu.

---

## Usage

### CLI menu (default)

```bash
make            # or: make menu_cli
```

### Graphical interface

```bash
make gui
```

### Run a specific module directly

Each module can be built and run on its own:

```bash
make 2x2        # 2x2 cube module
make 3x3        # 3x3 cube module
make korf       # Korf's algorithm
make kociemba   # Kociemba's algorithm
make lehmer     # Lehmer code utilities
```

### Passing arguments

Use `A="..."` to forward arguments to the program you are running:

```bash
make korf A="your args here"
```

Compiled binaries and object files are written to `build/`.

---

## Make Targets

### Run targets

| Target | Description |
|---|---|
| `menu_cli` | Interactive command-line menu (default) |
| `gui` | SplashKit graphical interface |
| `korf` | Korf's algorithm |
| `kociemba` | Kociemba's algorithm |
| `2x2` | 2x2 cube model |
| `3x3` | 3x3 cube model |
| `lehmer` | Lehmer code module |

### Data generation targets

| Target | Generates |
|---|---|
| `gen-corners` | Corner pattern database |
| `gen-edges-1` | First edge pattern database |
| `gen-edges-2` | Second edge pattern database |
| `gen-corners-slices` | Corner and slice database |
| `gen-edges-slices` | Edge and slice database |
| `gen-corners-slices-perm` | Corner slice-permutation database |
| `gen-edges-slices-perm` | Edge slice-permutation database |

### Maintenance targets

| Target | Description |
|---|---|
| `format` | Auto-format sources with `clang-format` |
| `format-check` | Fail if any file is not formatted |
| `tidy` | Run `clang-tidy` static analysis |
| `lint` | `format-check` + `tidy` |
| `clean` | Remove build artifacts |

---

## Pattern Databases

Korf's and Kociemba's algorithms rely on precomputed lookup tables that estimate how far a partial cube state is from solved. Generate them with the `gen-*` targets, for example:

```bash
make gen-corners
make gen-edges-1
make gen-edges-2
```

Generated tables are stored in the `data/` directory. Generating the larger tables can take a while and use significant memory, so run them once and reuse the results.

---

## Project Structure

```
.
├── src/            # C++ source files (solvers, cube models, CLI, GUI, data generation)
├── include/        # Header files
├── data/           # Generated pattern database files
├── Resources/      # Assets used by the GUI
├── logs/           # Log output
├── build/          # Build output (created on first build)
├── .vscode/        # VS Code workspace settings
├── .clang-format   # Code style configuration
├── .clang-tidy     # Static analysis configuration
├── Makefile        # Build, run, data generation and lint targets
└── README.md
```

### Source modules

| Module | Purpose |
|---|---|
| `menu_cli` | Command-line entry point and menu |
| `gui` | SplashKit graphical interface |
| `rubicks_2x2` | 2x2 cube representation and moves |
| `rubicks_3x3` | 3x3 cube representation and moves |
| `generate_scramble` | Random scramble generation |
| `depth_first_search` | Depth-first search routines |
| `korfs_algorithm` | Korf's pattern-database-guided solver |
| `kociembas_algorithm` | Kociemba's two-phase solver |
| `lehmer_code` | Permutation ranking and unranking |
| `generate_data` | Pattern database generation |

---

## How It Works

1. **State representation.** The cube is stored as piece permutations and orientations. Lehmer codes convert permutations into compact integer indices, which makes database lookups fast.
2. **Pattern databases.** Tables are precomputed by exploring the cube's state space outward from the solved state, recording the minimum number of moves needed to reach each partial state (such as corner positions or a subset of edges).
3. **Korf's algorithm.** An iterative-deepening search uses the databases as an admissible heuristic to prune branches that cannot lead to a shorter solution, which targets near-optimal results.
4. **Kociemba's algorithm.** The solver works in two phases. Phase 1 brings the cube into a restricted subgroup. Phase 2 finishes the solve using only moves that keep it in that subgroup, giving short solutions much faster than optimal search.

---

## Development

Format and lint before committing:

```bash
make format
make lint
```

Code style is defined in `.clang-format`, and static analysis checks in `.clang-tidy`.

To add a new runnable module, create `src/<name>.cpp` with its own `main()` and run it with `make run FILE=<name> DEPS="<other modules>"`. Modules other than the one being run have their `main()` renamed automatically at compile time, so they can be linked together.

---

## Troubleshooting

**`error: src/<name>.cpp does not exist`**
The `FILE` value does not match a file in `src/`. Check the spelling.

**`skm: command not found` when running `make gui`**
SplashKit is not installed or not on your `PATH`. See [Getting Started](#3-optional-install-splashkit-for-the-gui).

**Solver runs slowly or reports missing data**
Generate the required pattern databases first with the `gen-*` targets.

**Stale build behaviour**
Run `make clean` and rebuild.

---

## Future Improvements

- Step-by-step move animation controls (pause, speed adjustment)
- Custom colour themes for cube faces
- Benchmarks comparing Korf and Kociemba solve times and move counts
- Implementation of higher order Rubicks cubes

---

## Contributing

Issues and pull requests are welcome. Please run `make lint` before submitting changes and keep new code consistent with the existing style.

---

## License

No license has been specified yet. Add a `LICENSE` file (for example MIT) to clarify how others may use this project.
