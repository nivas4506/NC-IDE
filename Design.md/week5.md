# NC IDE - Week 5: VS Code Inspired Architecture

## Overview

The goal of Week 5 is to transform NC IDE into a modular, scalable, and VS Code-inspired development environment.

Instead of adding isolated features, NC IDE will adopt a Workbench Architecture similar to VS Code.

---

# Vision

NC IDE should provide:

* Fast startup
* Modular architecture
* Extension-first design
* Multi-language support
* Integrated developer workflow
* Professional desktop experience

---

# Main Layout

```text
--------------------------------------------------------
| Activity Bar | Sidebar | Editor Area                |
--------------------------------------------------------
| Explorer                                      |
| Search                                        |
| Source Control                                |
| Run & Debug                                   |
| Extensions                                    |
--------------------------------------------------------
| Terminal | Problems | Output | Debug Console  |
--------------------------------------------------------
| Status Bar                                    |
--------------------------------------------------------
```

---

# Features

## 1. Activity Bar

Located on the left side of the application.

Contains:

* Explorer
* Search
* Source Control
* Run & Debug
* Extensions

Responsibilities:

* Navigation between views
* Workspace management
* Quick switching

---

## 2. Explorer View

### Features

* Open Folder
* Open Workspace
* Create File
* Create Folder
* Rename
* Delete
* Drag & Drop

### Support

* Multi-root workspaces
* Recursive folders
* File icons

---

## 3. Search View

### Features

* Project-wide search
* Replace functionality
* Regex support
* Match case
* Whole word search

### Output

Display:

* File Name
* Line Number
* Matching Text

---

## 4. Source Control View

### Git Features

* Initialize Repository
* Commit
* Push
* Pull
* Branch Switch
* View History

### Display

* Modified Files
* Staged Changes
* Untracked Files

---

## 5. Run & Debug View

### Features

* Build Project
* Run Project
* Stop Program
* Debug Session

### Debug Controls

* Continue
* Step Over
* Step Into
* Step Out
* Breakpoints

---

## 6. Extensions View

### Capabilities

* Install Extensions
* Remove Extensions
* Update Extensions
* Enable/Disable Extensions

### Future Support

* NC Marketplace
* Third-party extensions
* Theme packages

---

## 7. Editor System

### Features

* Multi-tab editing
* Split editor
* Vertical split
* Horizontal split
* Breadcrumb navigation

### Editor Support

* C
* C++
* Python
* Java
* JavaScript
* HTML
* CSS
* JSON
* Markdown

---

## 8. Terminal Panel

### Tabs

* Terminal
* Problems
* Output
* Debug Console

### Features

* Integrated shell
* Command execution
* Build output
* Error logs

---

## 9. Status Bar

Display:

* Current Language
* Git Branch
* Line Number
* Column Number
* Encoding
* Notifications

---

## 10. Command Palette

Shortcut:

```text
Ctrl + Shift + P
```

### Capabilities

* Search Commands
* Open Files
* Execute Actions
* Settings Navigation

Examples:

* Open Folder
* Build Project
* Run Project
* Toggle Terminal

---

# Project Structure

```text
NC-IDE/
│
├── src/
│
├── core/
│   ├── ApplicationManager
│   ├── ServiceRegistry
│   └── EventBus
│
├── workbench/
│   ├── activitybar/
│   ├── sidebar/
│   ├── editor/
│   ├── panels/
│   └── statusbar/
│
├── explorer/
│
├── search/
│
├── scm/
│
├── debug/
│
├── terminal/
│
├── extensions/
│
├── workspace/
│
├── commands/
│
├── settings/
│
├── services/
│
└── assets/
```

---

# Design Principles

## Modular Architecture

Every feature should be independent.

Examples:

* Explorer Module
* Search Module
* Git Module
* Terminal Module

---

## Service-Oriented Design

Shared functionality should be exposed through services.

Examples:

* File Service
* Git Service
* Theme Service
* Workspace Service

---

## Extension-First Development

Future features should be installable as extensions.

Examples:

* AI Assistant
* Themes
* Language Packs
* Formatters

---

# Technical Stack

## Frontend

* Qt 6
* C++17

## Build System

* CMake

## Version Control

* Git

## Future Integrations

* Language Server Protocol (LSP)
* Debug Adapter Protocol (DAP)
* AI Assistant APIs

---

# Week 5 Deliverables

* Activity Bar
* Explorer View
* Search View
* Source Control View
* Run & Debug View
* Extensions View
* Terminal Panel
* Status Bar
* Command Palette
* Modular Architecture

---

# Expected Outcome

At the end of Week 5, NC IDE should visually and architecturally resemble VS Code and provide a strong foundation for:

* Extension Marketplace
* AI Coding Assistant
* Language Servers
* Advanced Debugging
* Cloud Development
* NC IDE v1.0
