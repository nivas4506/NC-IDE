cmake_minimum_required(VERSION 3.16)
project(NC-IDE VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Network)

set(SOURCES
    src/ai/AiAssistant.cpp
    src/ai/AiPanel.cpp
    src/commands/CommandRegistry.cpp
    src/core/App.cpp
    src/core/FileTypeRegistry.cpp
    src/core/FileManager.cpp
    src/core/ThemeManager.cpp
    src/core/NetworkManager.cpp
    src/core/NotificationManager.cpp
    src/debug/BuildManager.cpp
    src/debug/DebugManager.cpp
    src/debug/ProblemParser.cpp
    src/debug/RunDebugView.cpp
    src/debug/RunManager.cpp
    src/explorer/ExplorerView.cpp
    src/extensions/ExtensionManager.cpp
    src/extensions/ExtensionsView.cpp
    src/extensions/PluginLoader.cpp
    src/language/AutoCompleter.cpp
    src/language/CodeFormatter.cpp
    src/language/LspClient.cpp
    src/scm/CommitDialog.cpp
    src/scm/GitManager.cpp
    src/scm/SourceControlView.cpp
    src/search/ProjectSearchEngine.cpp
    src/search/SearchManager.cpp
    src/search/SearchView.cpp
    src/settings/SettingsManager.cpp
    src/settings/SettingsPanel.cpp
    src/tabs/EditorTab.cpp
    src/tabs/TabManager.cpp
    src/terminal/TerminalPanel.cpp
    src/workbench/activitybar/ActivityBar.cpp
    src/workbench/editor/BreadcrumbBar.cpp
    src/workbench/editor/CodeEditor.cpp
    src/workbench/editor/EditorArea.cpp
    src/workbench/editor/EditorGroup.cpp
    src/workbench/editor/LineNumberArea.cpp
    src/workbench/editor/SyntaxHighlighter.cpp
    src/workbench/panels/BottomPanel.cpp
    src/workbench/panels/DebugConsolePanel.cpp
    src/workbench/panels/ProblemsPanel.cpp
    src/workbench/sidebar/Sidebar.cpp
    src/workbench/statusbar/StatusBar.cpp
    src/workbench/CommandPalette.cpp
    src/workspace/SessionManager.cpp
    src/workspace/WorkspaceManager.cpp
    src/workspace/WorkspaceState.cpp
    src/main.cpp
    src/MainWindow.cpp
)

set(HEADERS
    src/ai/AiAssistant.h
    src/ai/AiPanel.h
    src/commands/CommandRegistry.h
    src/core/App.h
    src/core/FileTypeRegistry.h
    src/core/FileManager.h
    src/core/ThemeManager.h
    src/core/NetworkManager.h
    src/core/NotificationManager.h
    src/debug/BuildManager.h
    src/debug/DebugManager.h
    src/debug/ProblemParser.h
    src/debug/RunDebugView.h
    src/debug/RunManager.h
    src/explorer/ExplorerView.h
    src/extensions/ExtensionAPI.h
    src/extensions/ExtensionManager.h
    src/extensions/ExtensionsView.h
    src/extensions/PluginLoader.h
    src/language/AutoCompleter.h
    src/language/CodeFormatter.h
    src/language/LspClient.h
    src/scm/CommitDialog.h
    src/scm/GitManager.h
    src/scm/SourceControlView.h
    src/search/ProjectSearchEngine.h
    src/search/SearchManager.h
    src/search/SearchView.h
    src/services/ServiceLocator.h
    src/settings/SettingsManager.h
    src/settings/SettingsPanel.h
    src/tabs/EditorTab.h
    src/tabs/TabManager.h
    src/terminal/TerminalPanel.h
    src/workbench/activitybar/ActivityBar.h
    src/workbench/editor/BreadcrumbBar.h
    src/workbench/editor/CodeEditor.h
    src/workbench/editor/EditorArea.h
    src/workbench/editor/EditorGroup.h
    src/workbench/editor/LineNumberArea.h
    src/workbench/editor/SyntaxHighlighter.h
    src/workbench/panels/BottomPanel.h
    src/workbench/panels/DebugConsolePanel.h
    src/workbench/panels/ProblemsPanel.h
    src/workbench/sidebar/Sidebar.h
    src/workbench/statusbar/StatusBar.h
    src/workbench/CommandPalette.h
    src/workspace/SessionManager.h
    src/workspace/WorkspaceManager.h
    src/workspace/WorkspaceState.h
    src/MainWindow.h
)

add_executable(NC-IDE ${SOURCES} ${HEADERS})

target_include_directories(NC-IDE PRIVATE src)

target_link_libraries(NC-IDE PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Network
)

if(WIN32)
    set_target_properties(NC-IDE PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
