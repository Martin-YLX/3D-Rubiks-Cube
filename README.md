## 3D Rubik's Cube 

- **Project Name**: 3D Rubik's Cube  
- **Completion Time**: 2025  
- **Course**: Introduction to Intelligent Graphics and Image Processing  
- **Software Used**: Cursor, Visual Studio Code

## Overview

This is a lightweight 3D Rubik's Cube simulator built for educational purposes. Users can interact with a virtual cube using simple keyboard shortcuts to rotate faces by color. The project includes pre-built releases and shell scripts for easy compilation and execution.

## Features

- Interactive 3D Rubik's Cube
- Keyboard-controlled face rotation
- Lightweight build and execution scripts
- Release binaries available in `.pkg` and `.tar.gz` formats

## Controls

Each face of the cube corresponds to a key:

| Key | Face Color |
|-----|------------|
| `r` | Red        |
| `b` | Blue       |
| `o` | Orange     |
| `y` | Yellow     |
| `w` | White      |
| `g` | Green      |

Press the corresponding key to rotate that colored face clockwise.


## Usage

### Build the project

```bash
./make.sh
```

### Run the cube simulator

```bash
./run.sh
```

## Releases

You can download precompiled binaries from the [Releases](https://github.com/your-username/3D-Rubiks-Cube/releases) page:

- 📦 `3D-Rubiks-Cube.pkg` — Installer for macOS
- 📦 `3D-Rubiks-Cube.tar.gz` — Compressed archive for Unix-like systems

> Make sure to give execute permission to the scripts after extraction:
> ```bash
> chmod +x make.sh run.sh
> ```

## License

This project is licensed under the [MIT License](LICENSE).  
You are free to use, modify, and distribute this software for personal, academic, or commercial purposes, with proper attribution.
