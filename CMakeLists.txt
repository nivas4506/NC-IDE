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
