# NC IDE — Week 1 Build Specification
### Agent Build Prompt — Foundation Release (v0.1.0)

---

## 1. Mission Briefing

You are building **NC IDE**, a lightweight, fast, developer-first desktop IDE inspired by VS Code, CLion, and Zed. The guiding principles for every decision in this build are:

- **Clean architecture** — modules don't bleed into each other
- **Extensibility** — Week 1 code must not block Week 2+ features
- **Performance** — no unnecessary re-renders, no bloat, no blocking I/O on the main thread

This is **Week 1 of an ongoing build**. Ship a real, working foundation — not a mockup.

---

## 2. Deliverable Definition

> A desktop application named **NC IDE** that launches into a polished dark-themed IDE shell, where a user can create a new file, type code in a real multi-line editor, open an existing file from disk, and save it back — all through a professional menu/toolbar/status-bar layout.

If the build cannot do all five of those actions end-to-end, Week 1 is not done.

---

## 3. Recommended Tech Stack

| Layer | Choice | Why |
|---|---|---|
| Shell | **Electron** (or Tauri if binary size/perf is prioritized) | Native window, file system access, cross-platform |
| UI | **React + TypeScript** | Component isolation maps cleanly to the module architecture below |
| Editor Core | **Monaco Editor** (`@monaco-editor/react`) | VS Code's actual editor component — gives syntax highlighting architecture for free in later weeks |
| Styling | **CSS Modules** or **Tailwind** with design tokens (Section 5) | Theming must be swappable, not hardcoded |
| State | **Zustand** or **React Context** | Lightweight, no Redux ceremony for a Week 1 scope |
| Bundler | **Vite** | Fast dev loop |

> If a different stack is already in use in this repo, preserve the *architecture* (Section 6) regardless of language/framework substitutions.

---

## 4. Application Identity

- **App name:** `NC IDE` (window title, taskbar, About dialog, package name)
- **Window title format:** `{filename} — NC IDE` (e.g. `main.py — NC IDE`), or `Untitled — NC IDE` for unsaved files. Append `●` before the filename when there are unsaved changes (e.g. `● main.py — NC IDE`).
- **Startup experience:**
  - Window opens at a sensible default size (e.g. 1280×800), remembers last size/position on relaunch
  - No flash-of-white — load directly into the dark theme (set background color at the native window/BrowserWindow level before content paints)
  - Opens with one empty "Untitled" tab/document ready to type in

---

## 5. Design Language (Dark Tech Theme)

Implement these as **design tokens/CSS variables**, never hardcoded hex values inline. This is what makes Week 2 theming possible.

```css
:root {
  --color-background: #121212;   /* App shell background */
  --color-surface:    #1E1E1E;   /* Panels, sidebars, menus, status bar */
  --color-accent:     #1313EC;   /* Primary accent — active states, focus rings, buttons */
  --color-text:       #FFFFFF;   /* Primary text */
  --color-text-muted: #A0A0A0;   /* Secondary/disabled text, status bar info */
  --color-border:     #2A2A2A;   /* Panel dividers */

  --font-ui:    -apple-system, "Segoe UI", sans-serif;
  --font-mono:  "JetBrains Mono", "Fira Code", "Cascadia Code", monospace;

  --radius-sm: 4px;
  --transition-fast: 120ms ease;
}
```

**Design rules:**
- Minimalist, flat surfaces — no skeuomorphism, no heavy shadows
- The accent color (`#1313EC`) is used *sparingly*: active tab indicator, focus outlines, primary buttons, cursor/selection highlights — not as a background wash
- All interactive elements get a `transition: var(--transition-fast)` on hover/focus — this is the "smooth interactions" requirement
- Hover/active/focus states must be visually distinct (don't rely on a 5% opacity shift no one will see)

---

## 6. Modular Architecture

Lay out the source tree so each concern is isolated and each module exposes a clean public interface for the modules planned in later weeks (plugins, terminal, debugger, AI assistant, Git).

```
nc-ide/
├── src/
│   ├── main/                     # Electron main process
│   │   ├── main.ts                # App bootstrap, window creation
│   │   ├── menu.ts                 # Native menu bar construction
│   │   └── ipc/                    # IPC handlers (file ops, window state)
│   │
│   ├── modules/
│   │   ├── editor/                 # EDITOR MODULE
│   │   │   ├── EditorView.tsx       # Monaco wrapper component
│   │   │   ├── editor.store.ts      # Active document content/cursor/dirty state
│   │   │   └── editor.types.ts      # Document, Selection, EditorConfig types
│   │   │
│   │   ├── files/                  # FILE MODULE
│   │   │   ├── fileService.ts       # open/save/saveAs/create (Node fs calls via IPC)
│   │   │   ├── activeFile.store.ts  # Currently open file(s), dirty flags, recent files
│   │   │   └── file.types.ts
│   │   │
│   │   ├── ui/                     # UI MODULE
│   │   │   ├── MenuBar.tsx
│   │   │   ├── Toolbar.tsx
│   │   │   ├── StatusBar.tsx
│   │   │   ├── Workspace.tsx        # Layout shell composing the above + EditorView
│   │   │   └── shortcuts.ts         # Keyboard shortcut registry
│   │   │
│   │   └── theme/                  # THEME MODULE
│   │       ├── tokens.css           # Section 5 variables
│   │       ├── theme.types.ts       # Theme shape, for future light/custom themes
│   │       └── monacoTheme.ts       # Maps tokens.css → Monaco's defineTheme API
│   │
│   ├── shared/
│   │   ├── ipcChannels.ts          # Typed IPC channel constants shared main↔renderer
│   │   └── types.ts
│   │
│   ├── App.tsx
│   └── renderer-entry.tsx
│
├── electron-builder.json
├── package.json
└── README.md
```

**Hard architectural rules:**
1. `ui/` components never call `fs` directly — they call `files/fileService.ts`, which goes through IPC.
2. `editor/` knows nothing about menus or toolbars — it exposes `getValue()`, `setValue()`, `onChange`, `setDirty()` and that's it. The UI module wires those into menu actions.
3. `theme/` is the single source of truth for color/font tokens — no module defines its own colors.
4. Every module folder gets a one-line `index.ts` barrel export — this is the seam Week 2 plugins will hook into.

---

## 7. Feature Specification

### 7.1 Main Window
- [ ] Native menu bar (Section 7.2)
- [ ] Toolbar directly below the menu bar with icon buttons: **New, Open, Save, Undo, Redo** (icons + tooltips, no text labels needed)
- [ ] Status bar pinned to the window bottom showing: active file name, cursor position (`Ln {n}, Col {n}`), and a placeholder language indicator (e.g. `Plain Text`)
- [ ] Central workspace fills all remaining space and resizes responsively with the window (no fixed pixel widths on the editor container)

### 7.2 Menu System
Implement as **native OS menus** (Electron `Menu` API), not HTML dropdowns — this is what makes it feel like a real IDE.

| Menu | Items | Behavior |
|---|---|---|
| **File** | New, Open…, Save, Save As…, — , Exit | New → blank Untitled tab. Open → native file picker → loads into editor. Save → writes to known path, or behaves like Save As if untitled. Exit → closes app (prompt to save if dirty). |
| **Edit** | Undo, Redo, — , Cut, Copy, Paste | Delegate directly to Monaco's built-in command palette (`editor.trigger`) — don't reimplement clipboard logic. |
| **View** | (stub for Week 1 — e.g. "Toggle Sidebar" disabled/grayed, or "Zoom In/Out") | Exists and is wired into the menu, even if most items are placeholders — this is the seam for future panels. |
| **Help** | About NC IDE | Opens a small modal/dialog: app name, version, and a one-line tagline. |

### 7.3 Code Editor
- [ ] Multi-line text editor using Monaco (or equivalent) filling the workspace
- [ ] Monospace font (`var(--font-mono)`), sensible default size (14px), line numbers on
- [ ] Dark theme matching the token palette (custom Monaco theme — don't ship Monaco's default `vs-dark` unmodified; map it to `--color-background`/`--color-surface`/`--color-accent`)
- [ ] Language mode is **settable per file** even though no syntax highlighting rules ship yet beyond what Monaco provides out of the box (`.js`, `.py`, `.ts`, `.json`, etc. by extension) — this satisfies "syntax-ready architecture" without scope-creeping into a custom tokenizer
- [ ] Editor must support standard text editing: multi-cursor, undo/redo stack, select-all, line wrap toggle

### 7.4 File Management
- [ ] **New**: clears editor, resets active file to `Untitled`, clears dirty flag
- [ ] **Open**: native OS file dialog → reads file from disk → loads content into editor → sets active file path → updates window title and status bar
- [ ] **Save**: writes current editor content to the active file's path. If no path exists yet (Untitled), behaves like Save As.
- [ ] **Save As**: native save dialog → writes file → updates active file path/title
- [ ] **Active file tracking**: a single source of truth (`activeFile.store.ts`) holds `{ path: string | null, isDirty: boolean, content: string }`. The dirty flag flips `true` on any edit and `false` on successful save — drive the title-bar `●` and the Save button's enabled/disabled state off this one flag.
- [ ] Unsaved-changes guard: closing the app or opening a new file while dirty prompts **Save / Don't Save / Cancel**

### 7.5 User Experience & Keyboard Shortcuts
Wire these as real accelerators on the native menu items (so they show in the menu *and* work globally), not editor-only bindings:

| Action | Shortcut (Win/Linux) | Shortcut (macOS) |
|---|---|---|
| New File | `Ctrl+N` | `Cmd+N` |
| Open File | `Ctrl+O` | `Cmd+O` |
| Save | `Ctrl+S` | `Cmd+S` |
| Save As | `Ctrl+Shift+S` | `Cmd+Shift+S` |
| Undo | `Ctrl+Z` | `Cmd+Z` |
| Redo | `Ctrl+Y` / `Ctrl+Shift+Z` | `Cmd+Shift+Z` |
| Cut / Copy / Paste | `Ctrl+X/C/V` | `Cmd+X/C/V` |
| Exit | `Ctrl+Q` | `Cmd+Q` |

- [ ] All hover/focus states use the `--transition-fast` token — no instant/jarring state changes
- [ ] Disabled actions (e.g. Undo with empty history, Save with no changes) are visually grayed and non-interactive, not just non-functional

---

## 8. Acceptance Criteria (Definition of Done)

Run through this checklist literally before calling Week 1 complete:

1. App launches to a dark-themed window titled `Untitled — NC IDE` with zero white flash
2. Typing in the editor works immediately, with visible monospace text and line numbers
3. `File → New` clears the editor and resets the title
4. `File → Open…` opens a real OS dialog, loads a real file's contents correctly (including UTF-8/special characters), and updates the title bar
5. `File → Save` on an opened file writes changes back to the *same* path without re-prompting
6. `File → Save As…` on an Untitled doc prompts a save dialog and persists a new file to disk
7. Editing an opened file shows the `●` dirty indicator; saving clears it
8. `Edit → Undo/Redo/Cut/Copy/Paste` all function correctly against the editor
9. Closing the app with unsaved changes prompts Save/Don't Save/Cancel, and Cancel actually cancels the close
10. Status bar live-updates cursor line/column as the caret moves
11. Window is resizable and the editor reflows to fill the new space with no clipped or overflowing UI
12. No module imports across boundaries that Section 6 forbids (spot-check: grep `ui/` for `fs` or `electron` imports — should be none)

---

## 9. Explicitly Out of Scope for Week 1

Do not build these now — but every Week 1 decision should leave room for them:

- File tree / sidebar / multi-file project view
- Tabs for multiple simultaneously open files
- Real syntax highlighting / language servers
- Integrated terminal
- Debugger
- AI assistant panel
- Git integration
- Extension/plugin loading system
- Settings/preferences UI (a hardcoded theme is fine for now)

The `ui/`, `theme/`, and module barrel-export pattern in Section 6 exist specifically so these can be added later without refactoring Week 1's core.

---

## 10. Handoff Note

Build incrementally and verify each Acceptance Criterion item as you go rather than building everything then testing at the end. If any requirement above is ambiguous in a way that would change the file/folder structure, prefer the choice that keeps `editor/`, `files/`, `ui/`, and `theme/` independent — that boundary is the one architectural decision this entire roadmap depends on.