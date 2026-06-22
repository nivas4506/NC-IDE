# Implementation Plan — NC IDE Week 1 Foundation

Build and run a working desktop prototype of **NC IDE**, a lightweight, fast, developer-first IDE. Week 1 scope is the **application shell + code editor + file I/O**.

## User Review Required

We will set up the project directly in the workspace root (`c:\Users\A\OneDrive\Documents\NC IDE`) rather than a subfolder `nc-ide`, so that the project is immediately open and buildable from the workspace.

> [!IMPORTANT]
> - **Security Boundary**: We will enforce `nodeIntegration: false`, `contextIsolation: true` in the browser window, and expose a secure API bridge via a preload script `src/preload/preload.ts` for file operations.
> - **CodeMirror 6 Integration**: We will build a clean React wrapper for CodeMirror 6 using `@codemirror/view`, `@codemirror/state`, and the standard `codemirror` package. This ensures we can easily add syntax highlighting, theme styling, and keybindings in a modular way.

## Proposed Changes

### Configuration Files

#### [NEW] [package.json](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/package.json)
Contains NPM dependencies, scripts, and build configurations.

#### [NEW] [electron.vite.config.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/electron.vite.config.ts)
Vite configurations for electron main, preload, and renderer processes.

#### [NEW] [tsconfig.json](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/tsconfig.json)
Root TypeScript configuration.

#### [NEW] [tsconfig.node.json](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/tsconfig.node.json)
TypeScript configuration for Node processes (main/preload).

#### [NEW] [tsconfig.web.json](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/tsconfig.web.json)
TypeScript configuration for Renderer process.

### Electron Main Process

#### [NEW] [main.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/main/main.ts)
Electron application entry point, lifecycle events, and window creation.

#### [NEW] [menu.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/main/menu.ts)
Native application menu configuration (File, Edit, View, Help) with shortcuts.

#### [NEW] [fileHandlers.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/main/ipc/fileHandlers.ts)
Handles file system operations (Open, Save, Save As, New dialogs) and maps them to IPC listeners.

### Electron Preload Process

#### [NEW] [preload.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/preload/preload.ts)
Exposes safe file operation wrappers (`window.api.file`) to the renderer process via `contextBridge`.

### Renderer Process (Vite/React)

#### [NEW] [main.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/main.tsx)
Renderer entry point rendering the React application root.

#### [NEW] [App.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/App.tsx)
Main React layout component managing sidebar stubs, toolbar, status bar, and central editor area.

#### [NEW] [theme.css](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/theme/theme.css)
CSS custom properties defining the token system:
- `--nc-bg`: `#121212`
- `--nc-surface`: `#1E1E1E`
- `--nc-accent`: `#1313EC`
- `--nc-text`: `#FFFFFF`
- Monospace font stack (`JetBrains Mono`, `Fira Code`, `Consolas`, `monospace`)

#### [NEW] [ThemeProvider.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/theme/ThemeProvider.tsx)
React context provider for the theme, ensuring styles are systematically applied.

#### [NEW] [Editor.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/editor/Editor.tsx)
CodeMirror 6 wrapper component, mounting the editor, configuring dark style extension, and emitting text change events.

#### [NEW] [editorState.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/editor/editorState.ts)
Zustand or local state for the active buffer and editor state (cursor line/col, active document size).

#### [NEW] [fileManager.ts](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/files/fileManager.ts)
Manages opening, saving, and dirty tracking for the active file.

#### [NEW] [TitleBar.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/ui/TitleBar.tsx)
Top toolbar displaying app title, active file path, and dirty dot.

#### [NEW] [ToolBar.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/ui/ToolBar.tsx)
Toolbar containing quick action buttons (New, Open, Save, Save As).

#### [NEW] [StatusBar.tsx](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/renderer/ui/StatusBar.tsx)
Bottom status bar displaying file path, cursor position (line/col), and language mode.

---

## Verification Plan

### Automated Tests
* Run `npm run build` to verify main, preload, and renderer compile without type/build errors.

### Manual Verification
1. **Launch App**: Run `npm run dev`. Verify the window opens with a dark theme layout, menu bar, toolbar, editor, and status bar.
2. **New File**: Click `File -> New` or `Ctrl+N`. Verify the active document is empty and titled `Untitled`.
3. **Write & Save As**: Type text, press `Ctrl+Shift+S` (or `File -> Save As`), specify a target file `test.txt`. Confirm the file is written to disk and the title updates to `NC IDE — test.txt`.
4. **Open File**: Modify the file outside the app, then press `Ctrl+O` to open `test.txt`. Verify the file content loads correctly.
5. **Dirty Tracking & Save**: Edit text inside the editor. Verify the title changes to `NC IDE — test.txt •` and a dirty dot appears. Press `Ctrl+S` (or `File -> Save`). Verify the dirty dot disappears, the file updates, and no dialog is shown.
6. **Edit Actions**: Test Undo/Redo/Cut/Copy/Paste via native menu options and keyboard shortcuts (`Ctrl+Z`, `Ctrl+Y`, `Ctrl+X`, `Ctrl+C`, `Ctrl+V`).
7. **Exit**: Click `File -> Exit` or close the window. Verify the application exits cleanly.
