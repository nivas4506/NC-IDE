# NC IDE — Week 4 Specification
## Git Integration, Build & Run System, Command Palette, Workspace Management

**Project:** NC IDE — Modern Desktop Integrated Development Environment
**Stack:** Qt6, C++17
**Phase:** Week 4 of N
**Previous phases:** Week 1 (core editor shell), Week 2 (file explorer/UI), Week 3 (syntax highlighting, multi-tab editing, find/replace, project-wide search, auto-save, session restore)

---

## 1. Week 4 Goal

Transform NC IDE from a code editor into a complete developer workspace. Add Git source control, a multi-language build/run system, a Problems panel, a VS Code–style Command Palette, and persistent workspace state — so the full edit → build → run → debug → commit loop works inside the IDE.

---

## 2. Design Tokens (carry forward, do not redefine)

```
--bg-primary:    #121212
--bg-surface:    #1E1E1E
--accent:        #1313EC
--text-primary:  #E0E0E0
--text-muted:    #8A8A8A
--border:        #2A2A2A
--success:       #3FB950
--warning:       #D29922
--error:         #F85149
```

Apply `--success` to clean git status / passing builds, `--warning` to compiler warnings, `--error` to build errors and dirty/conflicted git state.

---

## 3. Architecture (additions to existing tree)

```
NC-IDE/
├── src/
│   ├── build/
│   │   ├── BuildManager.h
│   │   ├── BuildManager.cpp
│   │   ├── RunManager.h
│   │   └── RunManager.cpp
│   ├── git/
│   │   ├── GitManager.h
│   │   ├── GitManager.cpp
│   │   ├── CommitDialog.h
│   │   └── CommitDialog.cpp
│   ├── workspace/
│   │   ├── WorkspaceManager.h
│   │   ├── WorkspaceManager.cpp
│   │   ├── WorkspaceState.h
│   │   └── WorkspaceState.cpp
│   ├── palette/
│   │   ├── CommandPalette.h
│   │   ├── CommandPalette.cpp
│   │   ├── CommandRegistry.h
│   │   └── CommandRegistry.cpp
│   ├── problems/
│   │   ├── ProblemsPanel.h
│   │   ├── ProblemsPanel.cpp
│   │   ├── ProblemParser.h
│   │   └── ProblemParser.cpp
│   ├── project/        (existing — extend with template loader)
│   ├── explorer/        (existing — no breaking changes)
│   ├── terminal/        (existing — extend for run/build output piping)
│   ├── editor/          (existing — extend for jump-to-line API)
│   └── settings/        (existing)
│
├── templates/
│   ├── cpp/
│   ├── python/
│   ├── java/
│   ├── node/
│   └── web/
│
├── assets/
│   ├── icons/
│   └── themes/
│
└── docs/
```

### Module boundary rules (enforced, same convention as Week 1–3)
- `build/` and `git/` depend only on `core/` utilities and Qt — never on `editor/` internals directly. Communication happens via signals/slots.
- `palette/` may call into any manager (`BuildManager`, `GitManager`, `WorkspaceManager`, `explorer`) only through a `CommandRegistry` interface, never by holding direct references to UI widgets.
- `problems/` consumes parsed diagnostics via a `Diagnostic` struct — it does not know how those diagnostics were produced (compiler vs linter vs runtime).
- No circular includes between `build/`, `git/`, `workspace/`, `palette/`, `problems/`.

---

## 4. Feature Specifications

### 4.1 Git Integration

**Manager:** `GitManager` — wraps Git CLI via `QProcess`, never links libgit2 (CLI-first for portability).

**Supported actions:**
| Action | Git command invoked |
|---|---|
| Initialize Repository | `git init` |
| Clone Repository | `git clone <url> <path>` |
| Commit Changes | `git add -A && git commit -m "<msg>"` (staged subset supported) |
| View Git Status | `git status --porcelain=v2 -b` |
| Pull | `git pull --ff-only` (configurable) |
| Push | `git push` |
| Branch Management | `git branch`, `git checkout -b <name>`, `git switch <name>` |
| View Commit History | `git log --pretty=format:"%H|%an|%ad|%s" --date=short` |

**Parsing requirements:**
- `GitManager::parseStatus()` converts porcelain v2 output into a `QVector<GitFileStatus>` with fields: `path`, `state` (Added/Modified/Deleted/Renamed/Untracked/Conflicted), `staged` (bool).
- `GitManager::parseLog()` converts log output into `QVector<GitCommit>` (`hash`, `author`, `date`, `message`).
- All `QProcess` calls run async (`startDetached` for fire-and-forget, signal-based completion for status/log/commit) — never block the UI thread.

**UI components (Source Control Panel, sidebar tab):**
- **Git Sidebar icon** — branch icon in the activity bar, badge shows count of changed files.
- **Source Control Panel** — docked panel, header shows current branch name + sync (ahead/behind) indicator.
- **Changed Files List** — grouped into "Staged Changes" and "Changes", each row shows filename, relative path, and a colored status letter (M/A/D/U/R) matching `--warning`/`--success`/`--error`.
- **Commit Message Box** — multiline `QTextEdit` above a "Commit" button; disabled when empty or no staged files.
- **Current Branch Indicator** — persistent label in the status bar (bottom), click opens branch switcher popup.
- Clicking a changed file opens an inline diff view (unified diff via `git diff -- <file>`) in the editor area as a read-only tab.

**Signals:** `GitManager` emits `statusChanged()`, `commitCompleted(bool success, QString error)`, `branchChanged(QString name)`, `operationFailed(QString op, QString stderr)` — UI panel subscribes and refreshes reactively, never polls.

---

### 4.2 Build & Run System

**Managers:** `BuildManager` (compiles/builds), `RunManager` (executes built artifacts or interpreters).

**Supported languages and default commands:**

| Language | Build command | Run command |
|---|---|---|
| C | `gcc -g -Wall <file> -o <out>` | `./<out>` |
| C++ | `g++ -std=c++17 -g -Wall <file> -o <out>` | `./<out>` |
| Python | *(no build step)* | `python3 <file>` |
| Java | `javac <file>` | `java <ClassName>` |
| JavaScript (Node.js) | *(no build step)* | `node <file>` |

- Commands are defined in per-project `Run Configurations` (see below) and are user-editable, not hardcoded — the table above is just the default profile per detected language.
- `BuildManager::build()` and `RunManager::run()` execute via `QProcess`, streaming stdout/stderr live into the Build Output Panel / integrated terminal.
- `RunManager::stop()` terminates the running process tree (`QProcess::kill()` after `terminate()` grace period of 2s).

**Toolbar:**
- ▶ **Run** — runs current active configuration; disabled while a run is already in progress.
- ⚙ **Build** — triggers build for current language/project; spinner indicator while running.
- ■ **Stop** — enabled only while a process is active; kills it.

**Build Output Panel:**
- Docked panel (tabbed alongside Terminal and Problems).
- Streams raw stdout in default text color, stderr lines in `--error`.
- Auto-scrolls unless user has scrolled up manually.
- "Clear" button and a small status chip: `Building…` / `Build Succeeded` / `Build Failed (n errors)`.

**Run Configurations:**
- Stored per-project in `.ncide/runconfig.json`.
- Schema:
```json
{
  "configurations": [
    {
      "name": "Run main.cpp",
      "language": "cpp",
      "buildCommand": "g++ -std=c++17 -g -Wall ${file} -o ${fileDirname}/${fileBasenameNoExtension}",
      "runCommand": "${fileDirname}/${fileBasenameNoExtension}",
      "workingDirectory": "${workspaceFolder}",
      "env": {}
    }
  ],
  "activeConfiguration": "Run main.cpp"
}
```
- Supports variable substitution: `${file}`, `${fileDirname}`, `${fileBasenameNoExtension}`, `${workspaceFolder}`.
- Configuration dropdown lives next to the Run/Build/Stop toolbar buttons.

---

### 4.3 Problems Panel

**Module:** `ProblemParser` converts raw compiler/runtime output into a uniform `Diagnostic` struct: `{filePath, line, column, severity (Error/Warning/Info), message, source}`.

**Parsing targets (Week 4 scope):**
- GCC/G++ output format: `file:line:col: error|warning: message`
- Java `javac` output format: `file:line: error: message`
- Python traceback last-frame extraction: `File "path", line N, in <func>`
- Node.js stack trace first-frame extraction.

**Display:**
- Docked panel, grouped by file (collapsible tree: file → list of diagnostics).
- Each row shows severity icon (colored per token), line number, and message text.
- Clicking a row opens the file (if not open) and jumps the editor caret to `line:column`, with a transient highlight on that line.
- Panel header shows aggregate counts: `n Errors · n Warnings`.
- Runtime messages from `Run` (non-zero exit, uncaught exceptions) are appended into the same panel under an "Runtime" source group, separate from "Build".

---

### 4.4 Command Palette

**Shortcut:** `Ctrl+Shift+P` (cross-platform: also bind `Cmd+Shift+P` on macOS via Qt's `QKeySequence::StandardKey` abstraction where applicable, otherwise explicit mac binding).

**Module:** `CommandRegistry` is a flat, extensible registry of `Command { id, label, category, shortcut, std::function<void()> execute }`. Every manager (Git, Build, Run, Workspace, Theme, Explorer) registers its actions into this registry at startup — the palette itself has zero knowledge of what each command does.

**Palette UI (`CommandPalette`):**
- Modal-style overlay, centered top of window, fuzzy-search input + filtered list below.
- Fuzzy match against `label` and `category`, ranked by match quality (exact prefix > substring > fuzzy subsequence).
- Arrow keys navigate, `Enter` executes, `Esc` dismisses.
- Recently used commands shown first when input is empty.

**Minimum command set for Week 4:**
- Open Folder
- Build Project
- Run Project
- Stop Execution
- Toggle Terminal
- Toggle Problems Panel
- Toggle Source Control Panel
- Git: Commit
- Git: Pull
- Git: Push
- Git: Switch Branch
- Change Theme
- Save Workspace
- Load Workspace
- Go to File (delegates to existing Week 3 project-wide search/quick-open)

This is the floor, not the ceiling — every Week 4 UI action must also be reachable from the palette.

---

### 4.5 Workspace Management

**Module:** `WorkspaceManager` + `WorkspaceState` (extends Week 3's session-restore, does not replace it).

**Persisted per workspace** (`.ncide/workspace.json`):
```json
{
  "rootPath": "/path/to/project",
  "openTabs": [
    { "path": "src/main.cpp", "cursorLine": 42, "scrollPosition": 380 }
  ],
  "activeTab": "src/main.cpp",
  "terminalState": {
    "cwd": "/path/to/project",
    "history": ["npm install", "npm run build"]
  },
  "layout": {
    "explorerWidth": 260,
    "panelHeight": 220,
    "activeBottomPanel": "problems",
    "sourceControlPanelOpen": false
  },
  "activeRunConfiguration": "Run main.cpp"
}
```

**Behaviors:**
- **Save Workspace** — explicit action (palette/menu) and also auto-saved on clean app exit.
- **Load Workspace** — triggered on "Open Folder" if `.ncide/workspace.json` exists in that root; restores tabs, cursor positions, scroll, terminal cwd/history, panel layout, and last active run configuration.
- **Restore Open Tabs** — reopens files in original order; missing files are skipped with a non-blocking notification, not a crash.
- **Restore Terminal State** — reuses Week 3's terminal module, replays cwd and history buffer (not live process state — processes are never resumed).
- **Restore Project Layout** — panel sizes and which dock panel (Terminal/Problems/Source Control) was last focused.

---

### 4.6 File Type Detection

Extend existing Week 3 language-detection map with build/run association:

| Extension | Language | Highlighting | Build System | Run Config Default |
|---|---|---|---|---|
| `.c` | C | C | gcc | Run via compiled binary |
| `.h` | C header | C | — | — |
| `.cpp` | C++ | C++ | g++ | Run via compiled binary |
| `.hpp` | C++ header | C++ | — | — |
| `.py` | Python | Python | — | `python3` |
| `.java` | Java | Java | javac | `java` |
| `.js` | JavaScript | JavaScript | — | `node` |
| `.html` | HTML | HTML | — | Open in browser (external) |
| `.css` | CSS | CSS | — | — |
| `.json` | JSON | JSON | — | — |
| `.md` | Markdown | Markdown | — | — |

Detection is extension-first; a `FileTypeRegistry::detect(path)` returns a `FileTypeInfo{language, highlighter, buildSystem, defaultRunCommand}` consumed by both the editor (highlighting) and `BuildManager`/`RunManager` (defaults when no `runconfig.json` exists yet).

---

### 4.7 Project Templates

```
templates/
├── cpp/        → main.cpp, CMakeLists.txt (or plain Makefile), .ncide/runconfig.json
├── python/     → main.py, requirements.txt, .ncide/runconfig.json
├── java/       → Main.java, .ncide/runconfig.json
├── node/       → index.js, package.json, .ncide/runconfig.json
└── web/        → index.html, style.css, script.js
```

- "New Project from Template" command (palette + menu) prompts: template type → destination folder → project name.
- Each template ships a pre-filled `runconfig.json` so Build/Run works immediately on creation, with zero manual setup.
- Template instantiation does simple token replacement (`{{PROJECT_NAME}}`) in template files, then opens the new folder as workspace.

---

## 5. Cross-Cutting Technical Requirements

- Qt6, C++17, same toolchain as Weeks 1–3.
- All long-running work (`git`, build, run) happens off the UI thread via `QProcess` async signals — UI never blocks.
- Git CLI must be on `PATH`; if absent, Source Control Panel shows a clear "Git not found" state instead of crashing or silently failing.
- Cross-platform path handling (no hardcoded `/` or `\`) for Windows/macOS/Linux.
- All new modules documented with header comments (purpose, public API, ownership/lifetime notes) consistent with Weeks 1–3 conventions.
- No new module introduces a circular dependency with `editor/`, `explorer/`, or `terminal/`.

---

## 6. Deliverables Checklist

- [ ] Git Integration: init, clone, commit, status, pull, push, branch mgmt, history — all functional via `GitManager`
- [ ] Source Control Panel UI: sidebar, changed files list, commit box, branch indicator
- [ ] Build & Run system for C, C++, Python, Java, Node.js
- [ ] Toolbar: ▶ Run, ⚙ Build, ■ Stop wired to `RunManager`/`BuildManager`
- [ ] Build Output Panel with live streaming and status chip
- [ ] Run Configurations: `.ncide/runconfig.json` schema + variable substitution
- [ ] Problems Panel: parses GCC/G++, javac, Python, Node errors; click-to-jump; grouped by file
- [ ] Command Palette: `Ctrl+Shift+P`, fuzzy search, full command set from §4.4
- [ ] Workspace Management: save/load workspace, restore tabs/terminal/layout
- [ ] File type detection table extended with build/run association
- [ ] 5 project templates (cpp, python, java, node, web) with working default run configs
- [ ] `src/build/`, `src/git/`, `src/workspace/`, `src/palette/`, `src/problems/` modules created per architecture in §3
- [ ] No circular dependencies introduced; module boundaries respected
- [ ] Build instructions updated in `docs/`

---

## 7. Definition of Done

1. User can open any existing Git repo and see correct branch + status immediately.
2. User can stage, write a message, and commit from the Source Control Panel without touching a terminal.
3. User can pull/push and see success/failure reflected in the UI (not just console).
4. For each of the 5 supported languages, clicking Build then Run produces correct output in the Build Output Panel / terminal.
5. Stop reliably kills a running process (including infinite loops) with no zombie processes left behind.
6. Introducing a syntax error and building populates the Problems Panel with correct file/line, and clicking it jumps the caret to that exact location.
7. `Ctrl+Shift+P` opens the palette from anywhere in the app; every command in §4.4 executes correctly.
8. Closing and reopening a project workspace restores: open tabs, cursor positions, terminal history/cwd, panel layout, and active run configuration.
9. Creating a new project from any of the 5 templates produces a folder that builds and runs immediately with no manual config.
10. No UI thread blocking occurs during any Git, Build, or Run operation — app remains responsive throughout.

---

## 8. Out of Scope (explicitly deferred, not Week 4)

- Visual diff/merge conflict resolution UI (beyond raw diff display)
- Debugger (breakpoints, step execution) — likely Week 5+
- Remote Git hosting integration (GitHub/GitLab API, PRs, issues)
- Multi-root workspaces
- Build system plugins beyond the 5 languages listed