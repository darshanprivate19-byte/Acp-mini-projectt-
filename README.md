# 2D Graphics Editor in C

A terminal-based vector-graphics style 2D editor written in standard, portable C. It uses a 2D character grid as a canvas and supports drawing lines, rectangles, circles, and triangles. 

The editor is interactive, dynamically rendering the canvas and providing a menu system to add, delete, and modify shapes.

---

## Features

- **Vector-like Shape List**: Rather than writing destructively directly to a 2D array, the application stores a list of shapes. This allows shapes to be added, deleted, or modified individually at any time.
- **Canvas Rendering**:
  - Grid size: 60 columns wide ($\text{X} \in [0, 59]$) by 20 rows high ($\text{Y} \in [0, 19]$).
  - Background is rendered using `_` (dark gray).
  - Shapes are rendered using `*` (yellow).
  - The canvas displays grid labels (0-59 on X, 0-19 on Y) along the edges, making coordinate-based drawing easy.
- **Supported Shapes & Algorithms**:
  - **Line**: Drawn using *Bresenham's Line Algorithm* (integer-only arithmetic).
  - **Circle**: Drawn using the *Midpoint Circle Algorithm*.
  - **Rectangle**: Drawn as a wireframe box.
  - **Triangle**: Drawn by connecting three vertices with Bresenham lines.
- **Robust User Input**: Sanitizes and validates all console inputs to prevent buffer overflows or values that lie outside the boundaries of the canvas.
- **Interactive UI**: Leverages ANSI escape codes to clear the screen, position the cursor, and print colored text. If on Windows, the application automatically configures the terminal to support ANSI escape sequences.

---

## Project Structure

- `editor.c`: The core C source code containing data structures, drawing algorithms, and the interactive menu loop.
- `README.md`: This user guide and compilation documentation.

---

## Compilation Instructions

Since there is no default C compiler configured in the environment `PATH`, you can use any of the following standard toolchains to compile the code.

### 1. Using GCC (MinGW / MSYS2 / WSL)
Open your terminal (PowerShell, Command Prompt, or bash) and run:
```bash
gcc -O2 -Wall -Wextra editor.c -o editor.exe
```
Then run the compiled executable:
```bash
.\editor.exe
```

### 2. Using MSVC (Microsoft Visual Studio Compiler)
Open the **Developer Command Prompt for Visual Studio** (search "Developer Command Prompt" in the Windows Start menu) and run:
```cmd
cl /O2 /W4 editor.c /FEeditor.exe
```
Then run the compiled executable:
```cmd
editor.exe
```

### 3. Using Clang
Open your terminal and run:
```bash
clang -O2 -Wall -Wextra editor.c -o editor.exe
```
Then run:
```bash
.\editor.exe
```

---

## How to Use the Editor

When you start the program, you will see the 60x20 drawing canvas (filled with `_`), a list of current active shapes (initially empty), and the main menu:

1. **Add Shape**: Choose between a Line, Rectangle, Circle, or Triangle. You will be prompted to enter the coordinates (e.g. X: 0-59, Y: 0-19).
2. **Delete Shape**: Shows a list of active shapes with their unique IDs. Enter the ID of the shape you want to remove.
3. **Modify Shape**: Displays active shapes with their IDs. Enter a shape's ID to redefine its coordinates, radius, or dimensions.
4. **Clear Canvas**: Prompts for confirmation and deletes all shapes, resetting the canvas.
5. **Exit**: Exits the program and clears the console screen.
