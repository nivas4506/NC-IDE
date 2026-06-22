# Build Task: NC IDE — Week 1 Foundation

## Goal
Build and run a working desktop prototype of **NC IDE**, a lightweight, fast, developer-first IDE (in the spirit of VS Code, CLion, and Zed). Week 1 scope is the **application shell + code editor + file I/O** — nothing else. End state: I can launch the app, see a polished dark-theme IDE window, create/open/edit/save text files, and use the menu/toolbar/shortcuts to do it.

## Stack (decided — do not ask, do not substitute)
- **Electron** (desktop shell) + **React 18** + **TypeScript**
- **Vite** for the renderer build
- **CodeMirror 6** for the editor component (syntax-ready, performant, embeddable — lighter than Monaco for a Week 1 foundation, and explicitly chosen for future syntax-highlighting/LSP growth)
- Package manager: npm
- No backend/server process. All file I/O goes through Electron's main process via `ipcMain`/`contextBridge` (no `nodeIntegration: true`, use a proper preload script — this matters, don't skip it for speed).

## Architecture (must follow this module boundary — this is the foundation for everything after Week 1)
```
nc-ide/
  src/
    main/              # Electron main process
      main.ts           # app lifecycle, window creation
      menu.ts           # native menu bar (File/Edit/View/Help)
      ipc/
        fileHandlers.ts # open/save/save-as/new, dialog wiring
    preload/
      preload.ts        # contextBridge API surface exposed to renderer
    renderer/
      ui/               # UI module — shell, layout, titlebar, statusbar, toolbar
      editor/           # Editor module — CodeMirror wrapper, tab/active-file state
      files/            # File module — renderer-side file state, dirty tracking
      theme/            # Theme module — theme tokens + ThemeProvider
      App.tsx
      main.tsx
  package.json
  electron-builder config (or electron-vite) for packaging
```
Each module should be self-contained with a clear public interface (e.g. `editor/index.ts` exports a single `<Editor />` and a hook, not internals). This boundary is intentional groundwork for Week 2+ (terminal, debugger, AI assistant panel, Git panel, plugin host) — don't collapse modules into one giant component tree to save time.

## Theme tokens (apply via CSS variables in `theme/`, not hardcoded hex scattered through components)
```css
--nc-bg: #121212;
--nc-surface: #1E1E1E;
--nc-accent: #1313EC;
--nc-text: #FFFFFF;
```
Monospace font for the editor: system mono stack (`'JetBrains Mono', 'Fira Code', Consolas, monospace`) — fine if ligature fonts aren't bundled yet, just don't fall back to a serif/sans default.

## Features — Week 1 only

**Main window**
- Title: "NC IDE" (and reflect active filename + dirty-state dot/asterisk in the title bar, e.g. `NC IDE — app.ts •`)
- Menu bar, toolbar, status bar (show: active file path, cursor line/col, language placeholder), central editor workspace filling remaining space, resizable window with sane min-size.

**Menus**
- File: New, Open, Save, Save As, Exit
- Edit: Undo, Redo, Cut, Copy, Paste
- View: present, can be a stub (e.g. toggle status bar) — don't over-build
- Help: present, can be a stub "About NC IDE" dialog

**Keyboard shortcuts** (wire these — they're core to the IDE-feel, not optional polish):
`Ctrl/Cmd+N` New, `Ctrl/Cmd+O` Open, `Ctrl/Cmd+S` Save, `Ctrl/Cmd+Shift+S` Save As, `Ctrl/Cmd+Z/Y` Undo/Redo, `Ctrl/Cmd+X/C/V` Cut/Copy/Paste.

**Editor**
- Multi-line CodeMirror 6 instance, dark theme matching the token palette above, large workspace (fills available space, not a fixed small box).
- "Syntax-ready" = CodeMirror language extension slot wired up, even if only plain-text/JS highlighting ships this week. Don't build a custom highlighter.

**File management**
- New: opens an untitled, empty buffer.
- Open: native OS file dialog, load content into editor, track path.
- Save: writes current buffer to its known path; if untitled, behaves like Save As.
- Save As: native save dialog, writes content, updates active file path/title.
- Active file + dirty state tracked in renderer state and reflected in title bar + status bar.

## Explicitly out of scope this week (do not build, do not stub elaborately)
Terminal, debugger, Git integration, AI assistant panel, plugin/extension host, multi-tab editing, file tree/sidebar, settings UI. It's fine to leave clean extension points in the module structure above (that's the point of the architecture), but don't implement any of these features now.

## Definition of done — verify before reporting completion
1. `npm install && npm run dev` launches the app with no console errors.
2. Window shows dark theme, correct title, menu bar, toolbar, status bar, editor filling the workspace.
3. New → type text → Save As → pick a path → file is written to disk with that content (verify by reading the file back, not just trusting the dialog).
4. Open that same file → content loads correctly into the editor.
5. Edit it → title bar/status bar shows a dirty indicator → Save (no dialog) → dirty indicator clears.
6. Undo/Redo/Cut/Copy/Paste work both via menu and via keyboard shortcuts.
7. Exit (menu and window close) quits cleanly.
8. No `nodeIntegration: true`; file access only via the preload bridge.

Report back with: the file tree of what was created, and a short walkthrough of how each Definition of Done item was verified.