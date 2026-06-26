# Building NC IDE (v0.4.0)

## Prerequisites
- **Qt6** (Widgets, Core, Gui) — 6.2 or newer
- **CMake** 3.16+
- **C++17 Compiler** (MSVC 2019+, GCC 9+, or Clang 10+)
- **Git** installed on system PATH

## Windows (UCRT64 / MinGW MSYS2)
### Method 1: Using the PowerShell script (recommended on local environment)
This script runs `moc` manually and compiles all files directly for a fast compile iteration:
```powershell
.\build.ps1
```
The compiled binary will be placed at `build/NC-IDE.exe`. To run the application, make sure the MinGW bin directory (e.g. `C:\MinGW\ucrt64\bin`) is in your PATH, or run:
```powershell
.\build\NC-IDE.exe
```

### Method 2: Standard CMake Build
```powershell
mkdir build; cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
.\NC-IDE.exe
```

## Linux / macOS
```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6/lib/cmake
cmake --build . -j$(nproc)
./NC-IDE
```

## Configuration & Persistence Notes
- **Workspace State**: Stored in a local `.ncide/workspace.json` inside each project root folder.
- **Run Configurations**: Stored in `.ncide/runconfig.json` inside the project root folder.
- **Preferences**: Theme, editor font family, size, and auto-save configurations are stored via `QSettings` at OS-native locations (Registry on Windows, `~/.config/NC-IDE` on Linux).
