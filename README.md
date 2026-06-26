<div align="center">
  <h1>🚀 NC IDE</h1>
  <p><b>A modern, lightning-fast native code editor inspired by VS Code, built with WPF and C#.</b></p>
  <p>
    <a href="#about">About</a> •
    <a href="#tech-stack">Tech Stack</a> •
    <a href="#features--what-is-done">What's Done</a> •
    <a href="#roadmap--what-needs-to-be-done">Roadmap</a> •
    <a href="#contributing">Contributing</a>
  </p>
</div>

---

## About

**NC IDE** is an open-source, extensible, and high-performance Integrated Development Environment (IDE). Originally conceived as a C++ Qt6 application, it has been completely re-architected into a robust **C# WPF application** utilizing the MVVM pattern.

Our goal is to build a native editor that feels as powerful and familiar as VS Code, but leverages the rich ecosystem and native performance of the .NET Desktop framework.

## Tech Stack

- **Core Framework:** C# / .NET (WPF)
- **Editor Surface:** [AvalonEdit](http://avalonedit.net/) (for high-performance text editing and syntax highlighting)
- **Architecture Pattern:** MVVM (Model-View-ViewModel)
- **Language Intelligence:** Language Server Protocol (LSP) Integration

## Features & What is Done

We have successfully migrated to the new WPF architecture and laid a solid foundation for the IDE shell:

- **VS Code Inspired Layout:** Fully implemented Activity Bar, Sidebar, Editor Area, and Bottom Panel.
- **Workspace Explorer:** Lazy-loaded, high-performance file tree that instantly handles massive projects (like `node_modules`).
- **Editor Tabs:** Multi-tab support with dirty-state tracking (`*` unsaved indicators) and AvalonEdit two-way bindings.
- **Syntax Highlighting:** Dynamic language highlighting based on file extensions.
- **File System Operations:** Explorer context menus for creating files, creating folders, and safely deleting items.
- **LSP Foundation:** Foundational hooks for Language Server Protocol to support autocomplete, hover tooltips, and real-time diagnostics.

## Under the Hood (Snippets)

Here is a glimpse of how NC IDE efficiently handles massive file systems using a lazy-loading **Dummy Node Strategy** in the `WorkspaceManager`:

```csharp
// Models/FileTreeNode.cs
public class FileTreeNode : ObservableObject
{
    public string FullPath { get; set; }
    public bool IsDirectory { get; set; }
    public ObservableCollection<FileTreeNode> Children { get; } = new();

    private bool _isExpanded;
    public bool IsExpanded 
    {
        get => _isExpanded;
        set 
        {
            if (SetProperty(ref _isExpanded, value) && value && IsDummy)
            {
                OnExpanded?.Invoke(this); // Triggers real folder load
            }
        }
    }
}
```

And how dynamic **Syntax Highlighting** is seamlessly applied via AvalonEdit:

```csharp
// UI/Views/MainWindow.xaml.cs
private void ApplySyntaxHighlighting(string filePath)
{
    string extension = System.IO.Path.GetExtension(filePath);
    var definition = HighlightingManager.Instance.GetDefinitionByExtension(extension);
    CodeEditor.SyntaxHighlighting = definition;
}
```

## Roadmap & What Needs to be Done

NC IDE is growing fast, and there's plenty of exciting work ahead! Here are the immediate priorities:

- [ ] **Live Terminal Integration:** Connect the existing Terminal UI panel to a real background `Process` (e.g., PowerShell/CMD) with bi-directional standard input/output.
- [ ] **Command Palette:** Implement a `Ctrl+Shift+P` modal overlay with a fuzzy-searchable `CommandRegistry`.
- [ ] **Source Control (Git) Backend:** Wire up the `GitManager` to execute real `git status` and `git commit` commands, populating the Source Control sidebar.
- [ ] **Search Engine:** Implement a multi-threaded regex search across the workspace to power the Search sidebar view.
- [ ] **Run & Debug Engine:** Parse `.ncide/runconfig.json` files and build the process spawner for the "Run Project" button.
- [ ] **AI Assistant Integration:** Connect the mock Gemini UI panel to a real AI API backend.

## Contributing

**NC IDE is open source and we would love your help!** Whether you're fixing bugs, adding new features, or improving documentation, your contributions are welcome.

### How to Contribute

1. **Fork the repository**
2. **Clone your fork:** `git clone https://github.com/your-username/nc-ide.git`
3. **Open the project** in Visual Studio or JetBrains Rider (`NcIdeNative.csproj`).
4. **Create a new branch:** `git checkout -b feature/your-feature-name`
5. **Make your changes** and commit them: `git commit -m "Add some feature"`
6. **Push to the branch:** `git push origin feature/your-feature-name`
7. **Submit a Pull Request** to the `main` branch!

### Development Setup

- Ensure you have the **.NET 8+ SDK** (or .NET 10) installed.
- Restore packages and run the application via Visual Studio or via CLI:
  ```bash
  dotnet restore
  dotnet run --project "NC IDE"
  ```

---

<div align="center">
  <i>Built with ❤️ by the open-source community.</i>
</div>
