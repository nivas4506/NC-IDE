# NC IDE — Native C++ Prototype (v0.2.0)

NC IDE is a lightweight, fast, and highly modular desktop Integrated Development Environment built entirely in **C++** and **Qt6**. Following a full rewrite from the original Electron-based prototype, NC IDE now prioritizes native performance, lower memory footprint, and a tightly integrated modular architecture.

---

## 🚀 Tech Stack

| Layer | Technology | Purpose |
| --- | --- | --- |
| **Language** | C++17 | Core application logic and performance |
| **UI Framework** | Qt6 (Widgets) | Native OS windows, layout management, and UI components |
| **Build System** | CMake | Cross-platform build configuration |
| **Compiler** | MinGW GCC (UCRT64) | Compilation on Windows via MSYS2 |

---

## 📁 Modular Architecture

The source code conforms strictly to a modular architecture where different areas of concern are managed by distinct manager classes orchestrated by a central `MainWindow`.

```text
NC IDE/
├── CMakeLists.txt               # Main build configuration
├── build.ps1                    # PowerShell build script wrapper
├── resources/                   # Assets and JSON theme definitions
└── src/
    ├── main.cpp                 # Application entry point
    ├── MainWindow.cpp / .h      # Main window UI, menus, dock widgets, and orchestration
    ├── editor/                  # EDITOR MODULE
    │   ├── CodeEditor.cpp       # QPlainTextEdit subclass with line numbers/highlighting
    │   ├── SyntaxHighlighter    # Regex-based syntax highlighting logic
    │   └── LineNumberArea       # Custom widget for rendering line numbers
    ├── tabs/                    # TAB MANAGEMENT MODULE
    │   ├── TabManager.cpp       # Multi-tab logic, dirty state tracking, file I/O
    │   └── EditorTab.cpp        # Container for an individual CodeEditor and its state
    ├── search/                  # SEARCH MODULE
    │   ├── SearchManager.cpp    # In-file Find & Replace logic
    │   └── ProjectSearchEngine  # Multi-threaded search across directories
    ├── session/                 # SESSION MODULE
    │   └── SessionManager.cpp   # Auto-saves and restores open tabs across restarts
    └── settings/                # CONFIGURATION & THEME MODULE
        └── SettingsManager.cpp  # Loads themes and persists application settings
```

---

## ✨ Core Features & Components

### 1. Tab Management (`TabManager`)
Manages multiple open files simultaneously using `QTabWidget`. 
* Tracks unsaved changes (`isDirty`), appending a `*` to tab titles.
* Handles file opening, reading, saving, and "Save As" functionality.
* Warns users about unsaved changes before closing tabs or the application.

### 2. Code Editor (`CodeEditor` & `SyntaxHighlighter`)
A custom editor built on top of `QPlainTextEdit`.
* **Line Numbers:** A dynamic `LineNumberArea` tracks block counts and scrolling to render line numbers.
* **Current Line Highlighting:** Visually highlights the line the cursor is currently on.
* **Syntax Highlighting:** Uses `QSyntaxHighlighter` for regex-based, rule-driven code coloring.

### 3. Session State (`SessionManager`)
Ensures the developer never loses their workspace state.
* Periodically saves the list of currently open files and the active tab index.
* Automatically restores these tabs when the application is launched again.

### 4. Search Infrastructure (`SearchManager` & `ProjectSearchEngine`)
* **Local Search:** Fast find and replace functionality within the currently active document.
* **Project Search:** A robust `QThread`-based search engine that recursively scans directories for text patterns without blocking the main UI thread. Results are displayed in a native dockable window (`QDockWidget`).

### 5. Settings and Theming (`SettingsManager`)
* Implements dynamic JSON-based theming.
* Parses color hex codes and applies them globally via Qt Style Sheets (`QSS`) and specific palette modifications.

---

## 🛠️ How to Build and Run (Windows)

### Prerequisites
* **MSYS2**: Installed with the UCRT64 environment.
* Required MSYS2 packages: 
  ```bash
  pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-qt6
  ```

### Building the Project
You can build the project using the provided PowerShell script which wraps CMake commands:
```powershell
.\build.ps1
```
Alternatively, using CMake directly:
```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### Running the Application
A helper batch script is provided to ensure the necessary Qt DLLs are in the PATH before launching:
```cmd
.\build\run.bat
```
