# build.ps1
$ErrorActionPreference = "Stop"

$moc = "C:\MinGW\ucrt64\share\qt6\bin\moc.exe"
$cpp = "C:\MinGW\ucrt64\bin\g++.exe"

Write-Host "Creating build directory..."
if (!(Test-Path build)) { New-Item -ItemType Directory -Path build }

Write-Host "Running moc..."
& $moc -o src/moc_MainWindow.cpp src/MainWindow.h
& $moc -o src/moc_CodeEditor.cpp src/editor/CodeEditor.h
& $moc -o src/moc_SyntaxHighlighter.cpp src/editor/SyntaxHighlighter.h
& $moc -o src/moc_TabManager.cpp src/tabs/TabManager.h
& $moc -o src/moc_SearchManager.cpp src/search/SearchManager.h
& $moc -o src/moc_ProjectSearchEngine.cpp src/search/ProjectSearchEngine.h
& $moc -o src/moc_SessionManager.cpp src/session/SessionManager.h
& $moc -o src/moc_SettingsManager.cpp src/settings/SettingsManager.h

$sources = @(
    "src/main.cpp",
    "src/MainWindow.cpp",
    "src/editor/CodeEditor.cpp",
    "src/editor/LineNumberArea.cpp",
    "src/editor/SyntaxHighlighter.cpp",
    "src/tabs/TabManager.cpp",
    "src/tabs/EditorTab.cpp",
    "src/search/SearchManager.cpp",
    "src/search/ProjectSearchEngine.cpp",
    "src/settings/SettingsManager.cpp",
    "src/session/SessionManager.cpp",
    "src/moc_MainWindow.cpp",
    "src/moc_CodeEditor.cpp",
    "src/moc_SyntaxHighlighter.cpp",
    "src/moc_TabManager.cpp",
    "src/moc_SearchManager.cpp",
    "src/moc_ProjectSearchEngine.cpp",
    "src/moc_SessionManager.cpp",
    "src/moc_SettingsManager.cpp"
)

$includes = @(
    "-IC:/MinGW/ucrt64/include/qt6",
    "-IC:/MinGW/ucrt64/include/qt6/QtWidgets",
    "-IC:/MinGW/ucrt64/include/qt6/QtGui",
    "-IC:/MinGW/ucrt64/include/qt6/QtCore",
    "-Isrc"
)

$objects = @()

Write-Host "Compiling source files..."
foreach ($src in $sources) {
    $obj = "build/" + [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
    $objects += $obj
    Write-Host "Compiling $src -> $obj"
    & $cpp -std=c++17 -O2 $includes -c $src -o $obj
}

Write-Host "Linking executable..."
& $cpp $objects -LC:/MinGW/ucrt64/lib -lQt6Widgets -lQt6Gui -lQt6Core -mwindows -o build/NC-IDE.exe

Write-Host "Build successful! Executable created at build/NC-IDE.exe"
