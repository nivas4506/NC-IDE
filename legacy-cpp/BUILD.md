# Building NC IDE

## Prerequisites
- Qt6 (Widgets, Core, Gui) — 6.2 or newer
- CMake 3.16+
- A C++17 compiler (MSVC 2019+, GCC 9+, or Clang 10+)

## Linux / macOS
```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6/lib/cmake
cmake --build . -j$(nproc)
./NC-IDE
```

## Windows (MinGW / MSYS2 / MSVC)
### Using MinGW / ucrt64 (Recommended)
Make sure `cmake` and `qt6` are installed via MSYS2 pacman.
```powershell
mkdir build; cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
.\NC-IDE.exe
```

### Using MSVC + Qt6
```powershell
mkdir build; cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x/msvc2019_64"
cmake --build . --config Release
.\Release\NC-IDE.exe
```

## Notes
- If Qt6 isn't found, point `CMAKE_PREFIX_PATH` at your Qt installation's `lib/cmake` directory.
- Settings are stored via QSettings at the OS-native location (`~/.config/NC-IDE` on Linux, registry on Windows, `~/Library/Preferences` on macOS).
- Crash-recovery snapshots are written under the Qt `AppDataLocation` path — safe to delete manually if corrupted.
