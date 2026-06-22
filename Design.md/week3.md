# NC IDE — Week 3 Build Brief
**Target executor:** Antigravity (autonomous coding agent)
**Project:** NC IDE — Modern Desktop Integrated Development Environment
**Milestone:** Week 3 — Syntax Highlighting, Multi-Tab Editing, Search, Project Intelligence
**Stack:** Qt6, C++17, CMake
**Mode:** Modular, OOP, incremental — each module compiles and is testable in isolation before integration.

---

## 0. How to use this document

This is a build contract, not a vague feature list. Every module below has:
- A responsibility statement (what it owns, what it does NOT own)
- A finalized header (`.h`) — implement `.cpp` against this exact interface
- Integration points with other modules
- Acceptance criteria

Build in the dependency order given in §10 (Build Order). Do not skip ahead — `TabManager` depends on `CodeEditor`, `SearchManager` depends on `TabManager`, etc. Each module should compile cleanly before the next is started.

---

## 1. Final Directory Structure

```
NC-IDE/
├── CMakeLists.txt
├── BUILD.md
├── src/
│   ├── main.cpp
│   ├── MainWindow.h
│   ├── MainWindow.cpp
│   ├── editor/
│   │   ├── CodeEditor.h
│   │   ├── CodeEditor.cpp
│   │   ├── LineNumberArea.h
│   │   ├── LineNumberArea.cpp
│   │   ├── SyntaxHighlighter.h
│   │   └── SyntaxHighlighter.cpp
│   ├── tabs/
│   │   ├── TabManager.h
│   │   ├── TabManager.cpp
│   │   ├── EditorTab.h          (lightweight metadata struct per open file)
│   │   └── EditorTab.cpp
│   ├── search/
│   │   ├── SearchManager.h
│   │   ├── SearchManager.cpp
│   │   ├── ProjectSearchEngine.h
│   │   └── ProjectSearchEngine.cpp
│   ├── settings/
│   │   ├── SettingsManager.h
│   │   └── SettingsManager.cpp
│   └── session/
│       ├── SessionManager.h     (recent files, crash recovery, autosave persistence)
│       └── SessionManager.cpp
└── resources/
    ├── icons/
    └── themes/
        ├── dark.json
        └── light.json
```

---

## 2. Module: `SyntaxHighlighter` (editor/)

**Owns:** Token-level coloring rules for 7 languages. Re-runs on `QTextDocument` block changes only (not full-document rehighlight) for performance.

**Does NOT own:** caret position, file I/O, tab state.

### Language Detection
Detect language from file extension at tab-open time; pass into highlighter as enum. No content-sniffing.

| Extension | Language |
|---|---|
| `.c`, `.h` | C |
| `.cpp`, `.cc`, `.hpp`, `.hh` | C++ |
| `.py` | Python |
| `.java` | Java |
| `.js`, `.jsx`, `.mjs` | JavaScript |
| `.html`, `.htm` | HTML |
| `.css` | CSS |

### Header

```cpp
// src/editor/SyntaxHighlighter.h
#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <vector>

enum class Language {
    PlainText, C, Cpp, Python, Java, JavaScript, Html, Css
};

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument* parent, Language lang);

    void setLanguage(Language lang);
    Language language() const { return m_language; }

    // Applies theme colors (called by SettingsManager on theme change)
    void applyTheme(const QString& themeName);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void buildRulesForLanguage();
    void addRule(const QString& pattern, const QTextCharFormat& fmt);
    void highlightMultilineComments(const QString& text);
    void highlightMultilineStrings(const QString& text); // Python triple-quoted strings

    Language m_language;
    std::vector<HighlightRule> m_rules;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_preprocessorFormat;   // #include, #define (C/C++)

    QRegularExpression m_commentStartExpr;  // /* for C-family
    QRegularExpression m_commentEndExpr;    // */
    QRegularExpression m_tripleQuoteExpr;   // ''' or """ for Python

    // Block states for multiline constructs
    enum BlockState { Normal = 0, InComment = 1, InTripleString = 2 };
};
```

### Implementation notes
- One `HighlightRule` table per language, built once in `buildRulesForLanguage()` — switch on `m_language`.
- Keyword lists (minimum set, expand as needed):
  - **C**: `int char float double void short long unsigned signed const static struct union enum typedef sizeof if else for while do switch case break continue return goto extern volatile`
  - **C++** (C keywords plus): `class public private protected virtual override new delete this namespace template typename using try catch throw nullptr bool true false auto constexpr noexcept friend inline explicit operator`
  - **Python**: `def class import from as if elif else for while try except finally with return yield lambda pass break continue global nonlocal assert del raise in is not and or None True False self`
  - **Java**: `public private protected class interface extends implements static final void int double float boolean char long short byte if else for while do switch case break continue return new this super try catch finally throw throws import package abstract synchronized volatile transient instanceof`
  - **JavaScript**: `function var let const if else for while do switch case break continue return new this class extends super try catch finally throw typeof instanceof in of async await import export default null undefined true false`
  - **HTML**: tag names highlighted as keywords (regex on `<\/?[a-zA-Z][a-zA-Z0-9]*`), attribute names as types, attribute values as strings.
  - **CSS**: selectors as functions, property names as types, values/units as numbers, `!important` as keyword.
- Data types regex per language (e.g. C/C++: `\b(int|float|double|char|void|bool|long|short|unsigned|signed|size_t|auto)\b`).
- Function name heuristic: `\b[A-Za-z_][A-Za-z0-9_]*(?=\s*\()` — word immediately followed by `(`.
- Numbers: `\b[0-9]+\.?[0-9]*([eE][-+]?[0-9]+)?[fFLuU]?\b` plus hex `0[xX][0-9a-fA-F]+`.
- Strings: double-quoted `"(?:[^"\\]|\\.)*"` and single-quoted (skip single-quote for Python — handled by triple-quote logic + simple regex).
- Single-line comments: `//` (C-family, Java, JS), `#` (Python).
- Multiline comments (`/* */`) handled via `setCurrentBlockState` / `previousBlockState` pattern — standard Qt approach, implement exactly as Qt's official syntax highlighter example does.
- Python triple-quoted strings use the same block-state technique.
- Colors driven by theme JSON (see `SettingsManager` §6) — `applyTheme()` reloads `QTextCharFormat` colors and calls `rehighlight()`.

### Acceptance criteria
- Opening a `.py`, `.cpp`, `.java`, `.js`, `.html`, `.css`, `.c` file colors keywords, types, strings, comments, functions, numbers distinctly.
- Multiline `/* ... */` comments and Python `"""..."""` strings highlight correctly across line breaks.
- Switching theme (dark/light) re-colors all open tabs without reopening files.

---

## 3. Module: `CodeEditor` (editor/)

**Owns:** the actual text-editing widget — one instance per open tab. Wraps `QPlainTextEdit`.

**Does NOT own:** tab list, file path bookkeeping beyond its own file, search UI.

### Header

```cpp
// src/editor/CodeEditor.h
#pragma once
#include <QPlainTextEdit>
#include "SyntaxHighlighter.h"

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget* parent = nullptr);

    void setLanguage(Language lang);
    Language language() const;

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth() const;

    // Editor enhancements (§7)
    void duplicateLine();
    void deleteLine();
    void moveLineUp();
    void moveLineDown();
    void goToLine(int lineNumber);

    // Dirty-state tracking for tab indicators
    bool isModifiedSinceSave() const { return document()->isModified(); }

    // Cursor position display ("Ln 12, Col 4")
    QString cursorPositionLabel() const;

signals:
    void cursorPositionChanged2(const QString& label); // forwarded to status bar

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override; // Ctrl+D, Ctrl+G shortcuts handled here or in MainWindow

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);
    void matchBrackets();

private:
    LineNumberArea* m_lineNumberArea;
    SyntaxHighlighter* m_highlighter;
    Language m_language = Language::PlainText;

    void highlightMatchingBracket(QTextCursor cursor);
    static const QString BRACKET_PAIRS; // "()[]{}"
};
```

```cpp
// src/editor/LineNumberArea.h
#pragma once
#include <QWidget>

class CodeEditor;

class LineNumberArea : public QWidget {
public:
    explicit LineNumberArea(CodeEditor* editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    CodeEditor* m_codeEditor;
};
```

### Implementation notes
- Line numbers + current-line highlight: implement using Qt's standard "Code Editor Example" pattern (`lineNumberAreaWidth`, `updateLineNumberAreaWidth`, `highlightCurrentLine` via `QTextEdit::ExtraSelection`).
- Bracket matching: on cursor move, check char before/after cursor against `BRACKET_PAIRS`; if it's a bracket, scan forward/backward tracking nesting depth to find its pair; highlight both via `ExtraSelection` with a distinct background.
- `duplicateLine()` (Ctrl+D): copy current block text, insert a newline + copy below, keep cursor on original line.
- `deleteLine()`: select full block including newline, remove.
- `moveLineUp()` / `moveLineDown()`: swap current block with adjacent block text; reposition cursor to same column on the moved line.
- `goToLine(int)` (Ctrl+G): pair with a small `QInputDialog::getInt` prompt (built in `MainWindow`), then call `QTextCursor::movePosition(QTextCursor::Start)` + `Down` × (line-1), `setTextCursor`, `centerCursor`.
- Undo/Redo: `QPlainTextEdit` provides this natively (`undo()`/`redo()`) — just wire `Ctrl+Z`/`Ctrl+Y` (note: spec says Ctrl+Y for redo; Qt default is Ctrl+Shift+Z — explicitly rebind `QShortcut` for Ctrl+Y to `redo()` since this conflicts with no built-in Qt shortcut on Linux/Windows).

### Acceptance criteria
- Line numbers visible, resize correctly with font size changes.
- Current line has a subtle background highlight.
- Placing cursor next to `(`, `)`, `{`, `}`, `[`, `]` highlights its matching pair (or shows unmatched indicator if none).
- Ctrl+D duplicates line, Ctrl+G jumps to a line, line move/delete work via menu actions and shortcuts.

---

## 4. Module: `TabManager` (tabs/)

**Owns:** the set of open `CodeEditor` instances, their `QTabWidget` representation, unsaved-state indicators, tab reordering, open/close lifecycle.

**Does NOT own:** syntax rule definitions, search logic.

### Header

```cpp
// src/tabs/EditorTab.h
#pragma once
#include <QString>

struct EditorTab {
    QString filePath;      // empty if unsaved/new
    QString displayName;   // filename or "Untitled-N"
    bool isDirty = false;
    bool isNewFile = true;
};
```

```cpp
// src/tabs/TabManager.h
#pragma once
#include <QObject>
#include <QTabWidget>
#include <QMap>
#include "EditorTab.h"
#include "../editor/CodeEditor.h"

class TabManager : public QObject {
    Q_OBJECT
public:
    explicit TabManager(QTabWidget* tabWidget, QObject* parent = nullptr);

    CodeEditor* openFile(const QString& filePath);   // returns existing tab's editor if already open
    CodeEditor* newUntitledTab();
    bool closeTab(int index, bool promptIfDirty = true);
    void closeAllTabs(bool promptIfDirty = true);
    void closeOthers(int keepIndex);

    bool saveTab(int index);
    bool saveTabAs(int index, const QString& newPath);
    bool saveAll();

    CodeEditor* currentEditor() const;
    CodeEditor* editorAt(int index) const;
    QString filePathAt(int index) const;
    int indexOfFile(const QString& filePath) const;
    int tabCount() const;

    QList<EditorTab> allOpenTabs() const;     // for session persistence

signals:
    void tabDirtyStateChanged(int index, bool isDirty);
    void allTabsClosed();
    void activeFileChanged(const QString& filePath);

private slots:
    void onTabCloseRequested(int index);
    void onTabMoved(int from, int to);
    void onCurrentTabChanged(int index);
    void onEditorModified();

private:
    QTabWidget* m_tabWidget;
    QMap<int, EditorTab> m_tabData;
    int m_untitledCounter = 1;

    Language detectLanguageFromExtension(const QString& path) const;
    QIcon dirtyIndicatorIcon() const;     // small dot icon shown in tab title when unsaved
    void updateTabTitle(int index);
    bool promptSaveDiscardCancel(int index); // returns true = proceed with close
};
```

### Implementation notes
- `QTabWidget::setTabsClosable(true)`, `setMovable(true)` gives close buttons and drag-reorder for free — wire `tabCloseRequested` and `tabBar()->tabMoved` to the slots above.
- Unsaved indicator: prefix tab text with `● ` or use `setTabIcon` with a dot icon when `isDirty == true`; remove on save.
- `openFile()` first calls `indexOfFile()` — if already open, just `setCurrentIndex()` instead of duplicating.
- Closing a dirty tab triggers `QMessageBox` with Save / Discard / Cancel before proceeding (`promptSaveDiscardCancel`).
- "Close All" iterates tabs in reverse index order so indices remain valid during removal.
- On `onEditorModified()`, mark the tab dirty and emit `tabDirtyStateChanged`.

### Acceptance criteria
- Can open `main.cpp`, `app.py`, `index.html` simultaneously, switch via tab bar.
- Closing an unsaved tab prompts; "Close All" closes everything (with prompts as needed); dragging a tab reorders it.
- Tab title shows a dot/marker while there are unsaved changes, clears on save.

---

## 5. Module: `SearchManager` + `ProjectSearchEngine` (search/)

**Owns:** in-file find/replace logic and cross-project search. UI dialogs live in `MainWindow`; this module is pure logic + result data, kept testable without Qt widgets where possible.

### Header — in-file search

```cpp
// src/search/SearchManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QTextCursor>
#include "../editor/CodeEditor.h"

struct SearchOptions {
    bool matchCase = false;
    bool wholeWord = false;
    bool searchBackward = false;
};

class SearchManager : public QObject {
    Q_OBJECT
public:
    explicit SearchManager(QObject* parent = nullptr);

    void setActiveEditor(CodeEditor* editor);

    // Returns true if a match was found and selected in the editor
    bool findNext(const QString& term, const SearchOptions& opts);
    bool findPrevious(const QString& term, const SearchOptions& opts);

    int replaceCurrent(const QString& term, const QString& replacement, const SearchOptions& opts);
    int replaceAll(const QString& term, const QString& replacement, const SearchOptions& opts);

    int countMatches(const QString& term, const SearchOptions& opts) const;

signals:
    void searchWrapped();          // emitted when search loops back to start
    void noMatchFound();

private:
    CodeEditor* m_activeEditor = nullptr;
    QTextDocument::FindFlags buildFindFlags(const SearchOptions& opts, bool backward) const;
};
```

### Header — project-wide search

```cpp
// src/search/ProjectSearchEngine.h
#pragma once
#include <QObject>
#include <QString>
#include <QList>

struct ProjectSearchResult {
    QString filePath;
    int lineNumber;       // 1-based
    int columnNumber;     // 1-based
    QString lineText;      // the full matching line, for preview
};

class ProjectSearchEngine : public QObject {
    Q_OBJECT
public:
    explicit ProjectSearchEngine(QObject* parent = nullptr);

    void setProjectRoot(const QString& rootPath);
    void setFileFilters(const QStringList& extensions); // e.g. {"*.cpp","*.h","*.py"}

    // Runs synchronously for small/medium projects; emits progress for UI feedback
    QList<ProjectSearchResult> search(const QString& term, bool matchCase, bool wholeWord);

signals:
    void searchProgress(int filesScanned, int totalFiles);
    void searchFinished(int totalMatches);

private:
    QString m_projectRoot;
    QStringList m_fileFilters{"*.c","*.cpp","*.h","*.hpp","*.py","*.java","*.js","*.html","*.css"};

    QStringList collectProjectFiles() const; // recursive directory walk respecting filters
    QList<ProjectSearchResult> searchFile(const QString& path, const QRegularExpression& pattern) const;
};
```

### Implementation notes
- `findNext`/`findPrevious` use `QTextDocument::find()` with flags built from `SearchOptions`; on reaching document end, wrap to start and emit `searchWrapped()`, or emit `noMatchFound()` if nothing matches anywhere.
- Whole-word: use `QTextDocument::FindWholeWords` flag.
- `replaceAll` iterates with `findNext` in a loop starting from document start, replacing each match, returns count.
- `ProjectSearchEngine::collectProjectFiles()` uses `QDirIterator` with `QDirIterator::Subdirectories`, applying `m_fileFilters` as name filters; explicitly skip `.git`, `node_modules`, `build`, `.vscode` directories.
- `searchFile()` reads file line-by-line (`QTextStream`), regex-matches each line, records `ProjectSearchResult` per match with correct line/column.
- Project search results UI (in `MainWindow`): a dock widget list (`QListWidget` or `QTreeWidget`) grouped by file; double-click a result → `TabManager::openFile(path)` then `CodeEditor::goToLine(lineNumber)`.

### Keyboard shortcuts (wired in `MainWindow`)
- `Ctrl+F` → show inline Find bar / dialog, focus search field, pre-fill with selected text if any.
- `Ctrl+H` → show Find & Replace dialog.
- `Ctrl+Shift+F` (recommended addition) → open Project Search dock.

### Acceptance criteria
- Find highlights/selects matches, cycles through all occurrences, wraps around.
- Match case and whole word toggles change match behavior correctly.
- Replace and Replace All work and report a count.
- Project search returns results with correct file path + line number; clicking a result opens the file at that line.

---

## 6. Module: `SettingsManager` (settings/)

**Owns:** persisted user preferences. Backed by `QSettings` (INI or registry-native, cross-platform) rather than hand-rolled file parsing — more robust, but exposes a clean typed API so callers never touch `QSettings` directly.

### Header

```cpp
// src/settings/SettingsManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSettings>

class SettingsManager : public QObject {
    Q_OBJECT
public:
    static SettingsManager& instance(); // singleton — one settings store per app

    // Theme
    QString theme() const;                  // "dark" | "light"
    void setTheme(const QString& theme);

    // Font
    QString fontFamily() const;
    int fontSize() const;
    void setFontFamily(const QString& family);
    void setFontSize(int size);

    // Auto-save
    bool autoSaveEnabled() const;
    int autoSaveIntervalSeconds() const;     // configurable interval
    void setAutoSaveEnabled(bool enabled);
    void setAutoSaveIntervalSeconds(int seconds);

    // Recent files / projects (most-recent-first, capped at N entries)
    QStringList recentFiles() const;
    QStringList recentProjects() const;
    void addRecentFile(const QString& path);
    void addRecentProject(const QString& path);
    void clearRecentFiles();

    static constexpr int MAX_RECENT_ENTRIES = 15;

signals:
    void themeChanged(const QString& newTheme);
    void fontChanged(const QString& family, int size);
    void autoSaveSettingsChanged(bool enabled, int intervalSeconds);

private:
    SettingsManager();
    QSettings m_settings; // ~/.config/NC-IDE/NC-IDE.conf on Linux, registry on Windows

    void trimToMax(QStringList& list) const;
};
```

### Implementation notes
- Singleton via `static SettingsManager& instance()` returning a function-local `static SettingsManager s_instance;` — standard, thread-safety not a concern for a single-threaded UI settings store.
- `QSettings` group keys: `general/theme`, `general/fontFamily`, `general/fontSize`, `autosave/enabled`, `autosave/intervalSeconds`, `recent/files`, `recent/projects`.
- Defaults if unset: theme `"dark"`, font `"Consolas"` / `"JetBrains Mono"` fallback to `"Courier New"`, size `12`, autosave disabled, interval `30` seconds.
- Every setter calls `m_settings.sync()` (or relies on Qt's auto-sync) and emits the relevant change signal so `MainWindow`/`CodeEditor`/`SyntaxHighlighter` can react live (no restart required).

### Acceptance criteria
- Changing theme/font/autosave in a settings dialog persists across app restarts.
- Recently opened files/projects list is capped, deduplicated, most-recent-first.

---

## 7. Module: `SessionManager` (session/)

**Owns:** crash recovery and session restore — distinct from `SettingsManager` (preferences) because this is *state*, not *preference*.

### Header

```cpp
// src/session/SessionManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "../tabs/TabManager.h"

class SessionManager : public QObject {
    Q_OBJECT
public:
    explicit SessionManager(TabManager* tabManager, QObject* parent = nullptr);

    // Auto-save: periodically writes dirty buffers to a recovery cache
    // (NOT the original file — auto-save here means crash-recovery snapshotting,
    //  matching "recover unsaved work after crash" requirement)
    void startAutoSave(int intervalSeconds);
    void stopAutoSave();

    // Session persistence: which tabs were open, cursor positions, active tab
    void saveSession();
    void restoreSession();  // call on app startup

    bool hasRecoverableSession() const;
    void discardRecoverySnapshot();

private slots:
    void onAutoSaveTimeout();

private:
    TabManager* m_tabManager;
    QTimer m_autoSaveTimer;
    QString recoveryDirPath() const;   // e.g. QStandardPaths::AppDataLocation + "/recovery"
    QString sessionFilePath() const;   // JSON: list of {filePath, cursorLine, cursorCol}

    void writeRecoverySnapshot();
};
```

### Implementation notes
- Recovery snapshots are written per dirty buffer as `<recoveryDir>/<hash-of-path>.recovery`, containing raw unsaved text content. On `restoreSession()`, if a recovery file is newer than the source file's last-modified time, prompt the user "Unsaved changes were found — restore?" before loading.
- `saveSession()` writes a small JSON manifest (`session.json`) with open file paths, active tab index, and cursor positions — read back by `restoreSession()` on next launch to reopen the same tabs.
- Auto-save interval comes from `SettingsManager::autoSaveIntervalSeconds()`; `MainWindow` wires `SettingsManager::autoSaveSettingsChanged` to `SessionManager::startAutoSave/stopAutoSave`.
- This module deliberately does **not** silently overwrite the user's real files on disk — auto-save writes to the recovery cache only, since unconditionally rewriting files behind the user's back is poor editor behavior; "real" save still requires explicit Ctrl+S or the save action. (If true autosave-to-original-file is wanted instead, swap `writeRecoverySnapshot()` to call `TabManager::saveTab()` — flagged here as a design choice, not a silent assumption.)

### Acceptance criteria
- Enabling auto-save in settings begins periodic recovery snapshotting at the configured interval.
- Force-killing the app with unsaved changes, then relaunching, offers to restore the lost text.
- Relaunching restores the previously open tabs.

---

## 8. `MainWindow` — integration layer

Not a "module" per se, but the glue. Responsibilities:
- Hosts the `QTabWidget` (passed into `TabManager`), menu bar, toolbar, status bar (cursor position label, language indicator).
- Owns one `SettingsManager::instance()` reference, one `TabManager`, one `SearchManager`, one `ProjectSearchEngine`, one `SessionManager`.
- Wires all `QShortcut`/`QAction` bindings listed in §9.
- "Recent Files" quick-access menu reads from `SettingsManager::recentFiles()`, populated as a `QMenu` under File → Recent Files, each entry calling `TabManager::openFile()`.
- Project Search dock widget (`QDockWidget` wrapping a results list) toggled via shortcut/menu, populated by `ProjectSearchEngine::search()`.

---

## 9. Full Shortcut Map

| Shortcut | Action |
|---|---|
| Ctrl+F | Open Find |
| Ctrl+H | Open Find & Replace |
| Ctrl+Shift+F | Project-wide search |
| Ctrl+Z | Undo |
| Ctrl+Y | Redo |
| Ctrl+D | Duplicate line |
| Ctrl+G | Go to line |
| Ctrl+W | Close current tab |
| Ctrl+Shift+W | Close all tabs |
| Ctrl+S | Save current tab |
| Ctrl+Shift+S | Save as |
| Ctrl+Alt+S | Save all |
| Ctrl+Tab | Next tab |
| Ctrl+Shift+Tab | Previous tab |
| Alt+Up / Alt+Down | Move line up/down |
| Ctrl+Shift+K | Delete line |

---

## 10. Build Order (do not reorder)

1. `SettingsManager` (no dependencies) — implement + unit-test in isolation (QSettings round-trip).
2. `SyntaxHighlighter` (depends only on Qt) — test against static sample files per language.
3. `CodeEditor` (depends on `SyntaxHighlighter`) — verify line numbers, bracket match, line-edit ops standalone in a throwaway test window.
4. `TabManager` (depends on `CodeEditor`) — verify open/close/reorder/dirty-indicator with 3+ dummy files.
5. `SearchManager` (depends on `CodeEditor`/`TabManager`) — verify find/replace against an open tab.
6. `ProjectSearchEngine` (depends on filesystem only) — verify against the NC-IDE repo itself as a test project.
7. `SessionManager` (depends on `TabManager`, `SettingsManager`) — verify crash-kill + relaunch recovery manually.
8. `MainWindow` — wire everything, bind shortcuts, build menus/toolbar/status bar/docks last.

---

## 11. CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(NC-IDE VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/editor/CodeEditor.cpp
    src/editor/LineNumberArea.cpp
    src/editor/SyntaxHighlighter.cpp
    src/tabs/TabManager.cpp
    src/tabs/EditorTab.cpp
    src/search/SearchManager.cpp
    src/search/ProjectSearchEngine.cpp
    src/settings/SettingsManager.cpp
    src/session/SessionManager.cpp
)

set(HEADERS
    src/MainWindow.h
    src/editor/CodeEditor.h
    src/editor/LineNumberArea.h
    src/editor/SyntaxHighlighter.h
    src/tabs/TabManager.h
    src/tabs/EditorTab.h
    src/search/SearchManager.h
    src/search/ProjectSearchEngine.h
    src/settings/SettingsManager.h
    src/session/SessionManager.h
)

add_executable(NC-IDE ${SOURCES} ${HEADERS})

target_include_directories(NC-IDE PRIVATE src)

target_link_libraries(NC-IDE PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

if(WIN32)
    set_target_properties(NC-IDE PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
```

---

## 12. BUILD.md (build instructions deliverable)

```markdown
# Building NC IDE

## Prerequisites
- Qt6 (Widgets, Core, Gui) — 6.2 or newer
- CMake 3.16+
- A C++17 compiler (MSVC 2019+, GCC 9+, or Clang 10+)

## Linux / macOS
\`\`\`bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6/lib/cmake
cmake --build . -j$(nproc)
./NC-IDE
\`\`\`

## Windows (MSVC + Qt6)
\`\`\`powershell
mkdir build; cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x/msvc2019_64"
cmake --build . --config Release
.\Release\NC-IDE.exe
\`\`\`

## Notes
- If Qt6 isn't found, point `CMAKE_PREFIX_PATH` at your Qt installation's `lib/cmake` directory.
- Settings are stored via QSettings at the OS-native location (`~/.config/NC-IDE` on Linux, registry on Windows, `~/Library/Preferences` on macOS).
- Crash-recovery snapshots are written under the Qt `AppDataLocation` path — safe to delete manually if corrupted.
```

---

## 13. Definition of Done for Week 3

- [ ] All 7 languages highlight keywords/types/strings/comments/functions/numbers distinctly
- [ ] 3+ files can be open in tabs simultaneously, closable individually/all, reorderable, dirty-marked
- [ ] Ctrl+F find and Ctrl+H replace work with match-case and whole-word toggles
- [ ] Project-wide search returns clickable file+line results
- [ ] Line numbers, active-line highlight, bracket matching, cursor position in status bar all work
- [ ] Auto-save (configurable interval) + crash recovery restores unsaved text
- [ ] Recent files menu populated and functional; previous session restores on launch
- [ ] Undo/redo, duplicate/delete/move line, go-to-line all work via shortcuts
- [ ] Settings persist theme/font/autosave/recent-files/recent-projects across restarts
- [ ] Project builds cleanly via the provided CMakeLists.txt on a clean Qt6 install