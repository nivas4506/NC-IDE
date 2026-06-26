# Implementation Plan — NC IDE Week 5 Refactoring & Upgrades

Re-architect NC IDE into a modular, VS Code-inspired IDE. This plan outlines the structural refactoring of the codebase into functional services, a custom QSplitter workbench layout, and advanced features including split editors, a multi-root workspace explorer, global search/replace, and a local extensions manager.

---

## User Review Required

> [!IMPORTANT]
> - **Compilation Strategy**: The build system relies on a flat compilation script `build.ps1` listing objects. Restructuring sources into multiple subdirectories means we will need to update both `build.ps1` (to moc/compile the files in their new relative paths) and `CMakeLists.txt`. We must ensure all file paths are correctly registered.
> - **Split Editors**: Supporting horizontal/vertical splits means the single `m_tabWidget` will be replaced by an `EditorArea` container. This area manages a tree of `QSplitter`s containing `EditorGroup`s (each `EditorGroup` is a tab widget + breadcrumb bar). Files will open in the active group.
> - **Local Extensions**: Extensions will be managed by a custom `ExtensionManager` scanning a local directory (e.g. `<User_Home>/.ncide/extensions` or `.ncide/extensions` in the project) for `extension.json` manifests. Disabling/enabling an extension will write its status to settings.
> - **UI Theme**: Standardize on a highly styled VS Code Dracula Dark UI theme with rich styling, hover states, active indicators (a 2px left border on the active Activity Bar button), and clean typography.

---

## Proposed Changes

We will restructure `src/` to follow the VS Code architecture:

```
src/
├── core/
│   ├── main.cpp (moves from src/main.cpp)
│   ├── App.h / App.cpp (application context and core services)
│   └── FileTypeRegistry.h / FileTypeRegistry.cpp (moved from src/editor)
│
├── workbench/
│   ├── activitybar/
│   │   ├── ActivityBar.h / ActivityBar.cpp (vertical selector panel)
│   │   └── ActivityBarButton.h / ActivityBarButton.cpp
│   ├── sidebar/
│   │   └── Sidebar.h / Sidebar.cpp (stacked container for views)
│   ├── editor/
│   │   ├── EditorArea.h / EditorArea.cpp (split layout manager)
│   │   ├── EditorGroup.h / EditorGroup.cpp (single pane: tabwidget + breadcrumb)
│   │   ├── CodeEditor.h / CodeEditor.cpp (from src/editor)
│   │   ├── LineNumberArea.h / LineNumberArea.cpp (from src/editor)
│   │   ├── SyntaxHighlighter.h / SyntaxHighlighter.cpp (from src/editor)
│   │   └── BreadcrumbBar.h / BreadcrumbBar.cpp (clickable folder breadcrumbs)
│   ├── panels/
│   │   └── BottomPanel.h / BottomPanel.cpp (Terminal, Problems, Output, Debug Console)
│   └── statusbar/
│       └── StatusBar.h / StatusBar.cpp (Git, language, encoding, line/col, alerts)
│
├── explorer/
│   └── ExplorerView.h / ExplorerView.cpp (Multi-root tree + file manager ops)
│
├── search/
│   ├── SearchView.h / SearchView.cpp (Global Search / Replace panel)
│   ├── SearchManager.h / SearchManager.cpp (moved from src/search)
│   └── ProjectSearchEngine.h / ProjectSearchEngine.cpp (moved from src/search)
│
├── scm/
│   ├── SourceControlView.h / SourceControlView.cpp (staged/unstaged tree + diff + commit)
│   ├── GitManager.h / GitManager.cpp (moved from src/git)
│   └── CommitDialog.h / CommitDialog.cpp (moved from src/git)
│
├── debug/
│   ├── RunDebugView.h / RunDebugView.cpp (config selector + run/build/stop controls)
│   ├── BuildManager.h / BuildManager.cpp (moved from src/build)
│   └── RunManager.h / RunManager.cpp (moved from src/build)
│
├── terminal/
│   ├── TerminalPanel.h / TerminalPanel.cpp (moved from src/terminal)
│   └── TerminalService.h / TerminalService.cpp (powershell process host)
│
├── extensions/
│   ├── ExtensionsView.h / ExtensionsView.cpp (Extension manager UI panel)
│   └── ExtensionManager.h / ExtensionManager.cpp (Manifest scanner + state manager)
│
├── workspace/
│   ├── Workspace.h / Workspace.cpp (Multi-root workspace model)
│   ├── WorkspaceManager.h / WorkspaceManager.cpp (moved from src/workspace)
│   └── WorkspaceState.h / WorkspaceState.cpp (moved from src/workspace)
│
├── commands/
│   └── CommandRegistry.h / CommandRegistry.cpp (moved from src/palette)
│
├── settings/
│   └── SettingsManager.h / SettingsManager.cpp (moved from src/settings)
│
└── services/
    └── ServiceLocator.h (Global access to Git, Build, Run, Terminal, Extensions services)
```

---

### Core Window & Workbench Refactoring

#### [NEW] [ActivityBar.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/activitybar/ActivityBar.h) / [ActivityBar.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/activitybar/ActivityBar.cpp)
Vertical panel holding icon-like buttons for Explorer, Search, Source Control, Run & Debug, and Extensions. Handles click transitions.
- A vertical layout with 5 fixed-size icons (buttons).
- Custom paint event or CSS styling to draw an active indicator line (2px width, active color) on the left edge of the active button.
- Emits `activityChanged(int index)` when a button is selected. Toggles collapse if clicked again.

#### [NEW] [Sidebar.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/sidebar/Sidebar.h) / [Sidebar.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/sidebar/Sidebar.cpp)
Stacked widget container containing ExplorerView, SearchView, SourceControlView, RunDebugView, and ExtensionsView.
- Top header with Title and Sidebar Action Buttons (like "Collapse Sidebar").
- Emits signals on visibility toggling.

#### [NEW] [EditorArea.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/editor/EditorArea.h) / [EditorArea.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/editor/EditorArea.cpp)
Replaces `m_tabWidget` in the central panel. Manages a tree of `QSplitter`s containing one or more `EditorGroup`s.
- Supports splitting vertically/horizontally.
- Track active `EditorGroup` and redirect tab actions (`onNewFile`, `onOpenFile`, `onSave`) to it.
- Dynamically add/remove splitters as editor groups are created or closed.

#### [NEW] [EditorGroup.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/editor/EditorGroup.h) / [EditorGroup.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/editor/EditorGroup.cpp)
Represents a single editor panel containing:
- Clickable breadcrumbs path bar (`BreadcrumbBar`).
- Custom `QTabWidget` styled like VS Code tabs (dark tabs, active blue top-line, close button).
- Multi-tab management logic.

#### [NEW] [BreadcrumbBar.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/editor/BreadcrumbBar.h) / [BreadcrumbBar.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/editor/BreadcrumbBar.cpp)
Breadcrumb bar at the top of the editor.
- Parsing active file path into nodes (e.g. `workspace > src > workbench > editor > EditorArea.cpp`).
- Displaying each node as a button with a `>` separator.
- Clicking a segment displays a popup menu containing siblings in that folder for quick jump navigation.

#### [NEW] [BottomPanel.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/panels/BottomPanel.h) / [BottomPanel.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/panels/BottomPanel.cpp)
Contains Terminal, Problems, Output, and Debug Console. Extends previous bottom widget logic.

#### [NEW] [StatusBar.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/statusbar/StatusBar.h) / [StatusBar.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workbench/statusbar/StatusBar.cpp)
Customized status bar:
- Left: Git Branch (with click triggers branch checkout), workspace folder name.
- Right: Current Language (click opens selector), encoding, cursor line/col, action notification badge.

#### [MODIFY] [MainWindow.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/MainWindow.h) / [MainWindow.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/MainWindow.cpp)
Refactored to assemble the central QSplitter workbench layout:
- Left: ActivityBar.
- Splitter 1: Left is Sidebar, Right is Splitter 2.
- Splitter 2: Top is EditorArea, Bottom is BottomPanel.
- Bottommost: StatusBar.
- Serves as the high-level coordinator of actions and global command palette shortcuts.

---

### Component Views in Sidebar

#### [NEW] [ExplorerView.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/explorer/ExplorerView.h) / [ExplorerView.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/explorer/ExplorerView.cpp)
Project structure viewer. Supports:
- **Multi-Root Workspaces**: Custom QTreeWidget where each root directory is a top-level node.
- **Lazy loading**: Load subdirectories dynamically to support massive folders instantly.
- **File Management Ops**: Context menu with:
  - Create File / Folder
  - Rename
  - Delete (prompting confirmation)
  - Copy / Cut / Paste (storing path inside explorer instance buffer)
  - Refresh
- Clicking files opens them in the active editor area.

#### [NEW] [SearchView.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/search/SearchView.h) / [SearchView.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/search/SearchView.cpp)
Global search & replace UI panel in sidebar.
- Input boxes for: Search, Replace, Include/Exclude filters.
- Options: Case Match, Whole Word, Regex.
- Asynchronously query the `ProjectSearchEngine` and display matching occurrences grouped by file path.
- Double-clicking opens the file in the EditorArea and scrolls to the match location.
- Support "Replace" (single match) and "Replace All" (global replacement across files).

#### [NEW] [SourceControlView.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/scm/SourceControlView.h) / [SourceControlView.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/scm/SourceControlView.cpp)
Replaces `SourceControlPanel` in the new sidebar stacked layout.
- Grouping changes into: Staged, Unstaged, and Untracked.
- Dynamic diff preview tab: double-clicking a change generates a read-only git diff view.
- Buttons for: Commit (with message input), Push, Pull, Sync, Switch Branch.

#### [NEW] [RunDebugView.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/debug/RunDebugView.h) / [RunDebugView.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/debug/RunDebugView.cpp)
Run and debug controls in the sidebar.
- Selector dropdown for launch configurations.
- Button to open and edit `.ncide/runconfig.json`.
- Toolbar buttons: Compile/Build, Run/Execute, Cancel/Stop process.

#### [NEW] [ExtensionsView.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/extensions/ExtensionsView.h) / [ExtensionsView.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/extensions/ExtensionsView.cpp)
Simulated local extensions browser.
- Lists extensions loaded from `<user_dir>/.ncide/extensions` and built-in extensions.
- Shows extensions grouped by: Enabled, Disabled, Recommended.
- Extension detail dialog showcasing Name, ID, Version, Description, and Author.
- Action Buttons: Enable / Disable (toggles active state), Uninstall.
- Install Extension: input box or mock library browser to "install" mock extensions (such as `linter-cpp`, `formatter-prettier`, `dracula-theme`) that populate local JSON manifests.

#### [NEW] [ExtensionManager.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/extensions/ExtensionManager.h) / [ExtensionManager.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/extensions/ExtensionManager.cpp)
Model registry executing extension triggers:
- Scans `extensions` directories for folders with `extension.json` manifests:
  ```json
  {
    "id": "dracula-theme",
    "name": "Dracula Theme",
    "version": "1.0.0",
    "description": "Premium dark color palette for NC IDE.",
    "publisher": "Dracula Org",
    "contributes": {
      "commands": [
        { "command": "theme.setDracula", "title": "Preferences: Theme Dracula" }
      ]
    }
  }
  ```
- Keeps track of enabled/disabled plugins inside `SettingsManager`.
- Exposes activation/deactivation hooks. If disabled, its commands are unregistered or disabled.

---

### Services & Workspace Refactoring

#### [NEW] [ServiceLocator.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/services/ServiceLocator.h)
Globally accessible static container registering:
- GitManager (as a service)
- BuildManager / RunManager
- ExtensionManager
- CommandRegistry
- SettingsManager

#### [NEW] [Workspace.h](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workspace/Workspace.h) / [Workspace.cpp](file:///c:/Users/A/OneDrive/Documents/NC%20IDE/src/workspace/Workspace.cpp)
Data model supporting multi-root directories.
- Maintains a list of root folders: `QVector<QString> m_roots`.
- Serialization: Saves workspace folder structures to `.ncide-workspace` (JSON structure: `{ "folders": [ { "path": "path1" } ] }`).

---

## Verification Plan

### Automated Compilation Tests
1. **Source Registry**: Ensure all new files are accurately registered in `build.ps1` and `CMakeLists.txt`.
2. **Sequential Build**: Execute `.\build.ps1` in PowerShell. Confirm compilation completes successfully.
3. **Moc validation**: Verify Q_OBJECT headers are processed by moc.exe and objects link correctly.

### Manual Verification
1. **Workbench Layout**: Confirm the Activity Bar occupies the leftmost column. Clicking icons toggles the sidebar's visibility. Dragging the sidebar boundaries resizes it correctly.
2. **Editor Splitting**: Open a file. Select "Split Vertical". Verify a new EditorGroup pane is created. Verify tabs can move between panes. Verify clicking breadcrumb path nodes correctly triggers quick directories jumping.
3. **Multi-Root Explorer**: Add multiple directories via "Add Folder to Workspace". Confirm both roots are listed as top-level nodes in the Explorer view. Test right-click "Create File" and "Delete File" context actions.
4. **Global Search**: Search for a term across multiple files. Verify matching lines are displayed hierarchically. Confirm that replacing one or all occurrences updates the files.
5. **Extensions View**: Toggle Dracula Theme or Formatter from extensions. Verify enabling/disabling persists through application restarts.
