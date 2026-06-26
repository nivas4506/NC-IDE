# build.ps1
$ErrorActionPreference = "Stop"

$moc = "C:\MinGW\ucrt64\share\qt6\bin\moc.exe"
$cpp = "C:\MinGW\ucrt64\bin\g++.exe"

Write-Host "Creating build directory..."
if (!(Test-Path build)) { New-Item -ItemType Directory -Path build }

Write-Host "Running moc..."
# Core / Workbench mocs
& $moc -o src/moc_MainWindow.cpp src/MainWindow.h
& $moc -o src/moc_App.cpp src/core/App.h
& $moc -o src/moc_CodeEditor.cpp src/workbench/editor/CodeEditor.h
& $moc -o src/moc_SyntaxHighlighter.cpp src/workbench/editor/SyntaxHighlighter.h
& $moc -o src/moc_TabManager.cpp src/tabs/TabManager.h
& $moc -o src/moc_SearchManager.cpp src/search/SearchManager.h
& $moc -o src/moc_ProjectSearchEngine.cpp src/search/ProjectSearchEngine.h
& $moc -o src/moc_SettingsManager.cpp src/settings/SettingsManager.h
& $moc -o src/moc_GitManager.cpp src/scm/GitManager.h
& $moc -o src/moc_CommitDialog.cpp src/scm/CommitDialog.h
& $moc -o src/moc_SourceControlView.cpp src/scm/SourceControlView.h
& $moc -o src/moc_BuildManager.cpp src/debug/BuildManager.h
& $moc -o src/moc_RunManager.cpp src/debug/RunManager.h
& $moc -o src/moc_ProblemsPanel.cpp src/workbench/panels/ProblemsPanel.h
& $moc -o src/moc_TerminalPanel.cpp src/terminal/TerminalPanel.h
& $moc -o src/moc_CommandPalette.cpp src/workbench/CommandPalette.h
& $moc -o src/moc_WorkspaceManager.cpp src/workspace/WorkspaceManager.h
& $moc -o src/moc_SessionManager.cpp src/workspace/SessionManager.h

# Week 5 New Component Mocs
& $moc -o src/moc_ActivityBar.cpp src/workbench/activitybar/ActivityBar.h
& $moc -o src/moc_Sidebar.cpp src/workbench/sidebar/Sidebar.h
& $moc -o src/moc_EditorArea.cpp src/workbench/editor/EditorArea.h
& $moc -o src/moc_EditorGroup.cpp src/workbench/editor/EditorGroup.h
& $moc -o src/moc_BreadcrumbBar.cpp src/workbench/editor/BreadcrumbBar.h
& $moc -o src/moc_BottomPanel.cpp src/workbench/panels/BottomPanel.h
& $moc -o src/moc_StatusBar.cpp src/workbench/statusbar/StatusBar.h
& $moc -o src/moc_ExplorerView.cpp src/explorer/ExplorerView.h
& $moc -o src/moc_SearchView.cpp src/search/SearchView.h
& $moc -o src/moc_RunDebugView.cpp src/debug/RunDebugView.h
& $moc -o src/moc_ExtensionManager.cpp src/extensions/ExtensionManager.h
& $moc -o src/moc_ExtensionsView.cpp src/extensions/ExtensionsView.h

# Week 7 Mocs
& $moc -o src/moc_DebugManager.cpp src/debug/DebugManager.h
& $moc -o src/moc_AiAssistant.cpp src/ai/AiAssistant.h
& $moc -o src/moc_AiPanel.cpp src/ai/AiPanel.h
& $moc -o src/moc_NetworkManager.cpp src/core/NetworkManager.h
& $moc -o src/moc_LspClient.cpp src/language/LspClient.h
& $moc -o src/moc_SettingsPanel.cpp src/settings/SettingsPanel.h
& $moc -o src/moc_ThemeManager.cpp src/core/ThemeManager.h
& $moc -o src/moc_DebugConsolePanel.cpp src/workbench/panels/DebugConsolePanel.h

$sources = @(
    "src/main.cpp",
    "src/MainWindow.cpp",
    "src/core/App.cpp",
    "src/core/FileTypeRegistry.cpp",
    "src/workbench/activitybar/ActivityBar.cpp",
    "src/workbench/sidebar/Sidebar.cpp",
    "src/workbench/editor/EditorArea.cpp",
    "src/workbench/editor/EditorGroup.cpp",
    "src/workbench/editor/CodeEditor.cpp",
    "src/workbench/editor/LineNumberArea.cpp",
    "src/workbench/editor/SyntaxHighlighter.cpp",
    "src/workbench/editor/BreadcrumbBar.cpp",
    "src/workbench/panels/BottomPanel.cpp",
    "src/workbench/panels/ProblemsPanel.cpp",
    "src/workbench/statusbar/StatusBar.cpp",
    "src/workbench/CommandPalette.cpp",
    "src/tabs/TabManager.cpp",
    "src/tabs/EditorTab.cpp",
    "src/explorer/ExplorerView.cpp",
    "src/search/SearchView.cpp",
    "src/search/SearchManager.cpp",
    "src/search/ProjectSearchEngine.cpp",
    "src/scm/SourceControlView.cpp",
    "src/scm/GitManager.cpp",
    "src/scm/CommitDialog.cpp",
    "src/debug/RunDebugView.cpp",
    "src/debug/BuildManager.cpp",
    "src/debug/RunManager.cpp",
    "src/debug/ProblemParser.cpp",
    "src/terminal/TerminalPanel.cpp",
    "src/extensions/ExtensionsView.cpp",
    "src/extensions/ExtensionManager.cpp",
    "src/commands/CommandRegistry.cpp",
    "src/settings/SettingsManager.cpp",
    "src/workspace/WorkspaceState.cpp",
    "src/workspace/WorkspaceManager.cpp",
    "src/workspace/SessionManager.cpp",
    "src/debug/DebugManager.cpp",
    "src/ai/AiAssistant.cpp",
    "src/ai/AiPanel.cpp",
    "src/core/NetworkManager.cpp",
    "src/language/LspClient.cpp",
    "src/settings/SettingsPanel.cpp",
    "src/core/ThemeManager.cpp",
    "src/workbench/panels/DebugConsolePanel.cpp",
    
    # Generated mocs
    "src/moc_MainWindow.cpp",
    "src/moc_App.cpp",
    "src/moc_CodeEditor.cpp",
    "src/moc_SyntaxHighlighter.cpp",
    "src/moc_TabManager.cpp",
    "src/moc_SearchManager.cpp",
    "src/moc_ProjectSearchEngine.cpp",
    "src/moc_SettingsManager.cpp",
    "src/moc_GitManager.cpp",
    "src/moc_CommitDialog.cpp",
    "src/moc_SourceControlView.cpp",
    "src/moc_BuildManager.cpp",
    "src/moc_RunManager.cpp",
    "src/moc_ProblemsPanel.cpp",
    "src/moc_TerminalPanel.cpp",
    "src/moc_CommandPalette.cpp",
    "src/moc_WorkspaceManager.cpp",
    "src/moc_SessionManager.cpp",
    "src/moc_ActivityBar.cpp",
    "src/moc_Sidebar.cpp",
    "src/moc_EditorArea.cpp",
    "src/moc_EditorGroup.cpp",
    "src/moc_BreadcrumbBar.cpp",
    "src/moc_BottomPanel.cpp",
    "src/moc_StatusBar.cpp",
    "src/moc_ExplorerView.cpp",
    "src/moc_SearchView.cpp",
    "src/moc_RunDebugView.cpp",
    "src/moc_ExtensionManager.cpp",
    "src/moc_ExtensionsView.cpp",
    "src/moc_DebugManager.cpp",
    "src/moc_AiAssistant.cpp",
    "src/moc_AiPanel.cpp",
    "src/moc_NetworkManager.cpp",
    "src/moc_LspClient.cpp",
    "src/moc_SettingsPanel.cpp",
    "src/moc_ThemeManager.cpp",
    "src/moc_DebugConsolePanel.cpp"
)

$includes = @(
    "-IC:/MinGW/ucrt64/include/qt6",
    "-IC:/MinGW/ucrt64/include/qt6/QtWidgets",
    "-IC:/MinGW/ucrt64/include/qt6/QtGui",
    "-IC:/MinGW/ucrt64/include/qt6/QtCore",
    "-IC:/MinGW/ucrt64/include/qt6/QtNetwork",
    "-Isrc",
    "-Isrc/services"
)

$objects = @()

Write-Host "Compiling source files..."
foreach ($src in $sources) {
    $obj = "build/" + [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
    $objects += $obj
    
    $needsCompile = $true
    if (Test-Path $obj) {
        $srcTime = (Get-Item $src).LastWriteTime
        $objTime = (Get-Item $obj).LastWriteTime
        # If there is a generated moc or source file change, rebuild
        if ($srcTime -lt $objTime) {
            $needsCompile = $false
        }
    }
    
    if ($needsCompile) {
        Write-Host "Compiling $src -> $obj"
        & $cpp -std=c++17 -O2 $includes -c $src -o $obj
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Compilation failed for $src"
            exit $LASTEXITCODE
        }
    } else {
        # Keep output quiet or print skip
        Write-Host "Skipping $src (up to date)"
    }
}

Write-Host "Linking executable..."
& $cpp $objects -LC:/MinGW/ucrt64/lib -lQt6Widgets -lQt6Gui -lQt6Core -lQt6Network -mwindows -o build/NC-IDE.exe

Write-Host "Build successful! Executable created at build/NC-IDE.exe"
