# fp_text_editor2

A lightweight, high-performance console text editor written in **C++20** with **zero external dependencies**. This project serves as a demonstration of low-level systems programming, featuring custom reimplementations of fundamental data structures and cross-platform terminal handling.

## 🚀 Features

* **Zero Dependencies**: The project does not use `std::vector`, `std::string`, or any other standard template library (STL) containers.
* **Custom STL Reimplementations**:
    * **`spd::Vector<T>`**: A dynamic array with move semantics, growth factors, and manual memory tracking.
    * **`spd::GapBuffer`**: A high-efficiency buffer used for each line, allowing $O(1)$ character insertion and deletion at the cursor.
    * **`spd::StringView`**: A lightweight, non-owning character buffer abstraction.
* **Efficient Line Management**: Uses a `Vector` of `GapBuffer` objects to handle multi-line text editing efficiently.
* **Cross-Platform Terminal Engine**: Custom abstraction for **Windows** (Win32 Console API) and **Linux** (POSIX termios) to handle raw mode, ANSI escape sequences, and hardware input events.
* **Manual Memory Tracking**: Custom allocation macros (`SPD_ALLOC`, `SPD_FREE`) that track total bytes allocated and freed in real-time.

## 🏗 Architecture

### Custom Containers (`src/stl/`)
* **`vector.hpp`**: Implements dynamic resizing and element management without `std::vector`.
* **`gap_buff.cpp`**: Manages the "gap" in line data to minimize memory shifts during typing.
* **`iterator.hpp`**: Custom pointer-based iterators for traversing project containers.

### Core Logic (`src/core/`)
* **`editor.cpp`**: Coordinates rendering, input processing, and high-level commands like merging lines.
* **`terminal.cpp`**: Handles the complexities of "Raw Mode" (disabling echo and line buffering) across different operating systems.
* **`input.cpp`**: Maps hardware key codes to logical editor commands.
* **`mem.h`**: Provides the tracked allocation system to monitor memory usage.

## ⌨️ Controls

| Key | Action |
| :--- | :--- |
| **Arrow Keys** | Navigate the cursor |
| **Enter** | Break the current line and move text after the cursor down |
| **Backspace** | Delete character before the cursor or merge with the line above |
| **Delete** | Delete character at the cursor or merge with the line below |
| **Home / End** | Jump to the beginning or end of the current line |
| **Escape** | Exit the editor |

## 🛠 Building the Project

The project uses CMake for cross-platform builds. Ensure you have a C++20 compatible compiler.

### Windows (Visual Studio)
1.  Create a build directory: `mkdir build && cd build`
2.  Generate the solution: `cmake ..`
3.  Build the project:
    * **Debug**: `cmake --build . --config Debug`
    * **Release**: `cmake --build . --config Release`

### Linux (Make)
1.  Create a build directory: `mkdir build && cd build`
2.  Generate Makefiles:
    * **Debug**: `cmake -DCMAKE_BUILD_TYPE=Debug ..`
    * **Release**: `cmake -DCMAKE_BUILD_TYPE=Release ..`
3.  Compile: `make`

## 🧪 Unit Tests
The project includes internal unit tests for the custom `Vector` and `GapBuffer` implementations to ensure memory safety and algorithmic correctness. These can be enabled or disabled in `src/main.cpp`.

## 📜 License
This project is developed for educational purposes, demonstrating low-level data structure design and cross-platform systems programming.