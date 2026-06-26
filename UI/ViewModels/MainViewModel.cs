using System;
using System.Linq;
using System.Windows.Input;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Microsoft.Win32;
using NcIdeNative.Engines;
using NcIdeNative.ExtensionMarketplace;
using NcIdeNative.ExtensionMarketplace.Api;
using NcIdeNative.Models;
using NcIdeNative.Core;
using NcIdeNative.BackendServices;

namespace NcIdeNative.UI.ViewModels;

public class MainViewModel : ObservableObject
{
    private readonly FileManager _fileManager;
    private readonly WorkspaceManager _workspaceManager;
    private readonly TerminalService _terminalService;
    private readonly AiAssistant _aiAssistant;
    private readonly SearchService _searchService;
    private readonly GitService _gitService;
    private readonly ExecutionService _executionService;
    private readonly SettingsManager _settingsManager;
    private string? _currentWorkspacePath;
    
    public ObservableCollection<EditorTab> OpenTabs { get; } = new ObservableCollection<EditorTab>();

    private EditorTab? _selectedTab;
    public EditorTab? SelectedTab
    {
        get => _selectedTab;
        set => SetProperty(ref _selectedTab, value);
    }

    public ICommand OpenFileCommand { get; }
    public ICommand NewFileCommand { get; }
    public ICommand SaveFileCommand { get; }
    public ICommand CloseTabCommand { get; }
    public ICommand CloseCurrentTabCommand { get; }
    public ICommand ExitCommand { get; }
    public ICommand OpenFolderCommand { get; }
    public ICommand AddNewFileCommand { get; }
    public ICommand AddNewFolderCommand { get; }
    public ICommand DeleteFileCommand { get; }
    public ICommand ToggleAiPanelCommand { get; }
    public ICommand SendAiMessageCommand { get; }
    public ICommand ToggleTerminalCommand { get; }
    public ICommand SendTerminalCommand { get; }
    public ICommand ToggleExtensionsPanelCommand { get; }
    public ICommand ToggleThemeCommand { get; }
    public ICommand SetSidebarViewCommand { get; }
    public ICommand ExecuteSearchCommand { get; }
    public ICommand RefreshGitCommand { get; }
    public ICommand CommitGitCommand { get; }
    public ICommand RunProjectCommand { get; }
    public ICommand OpenSettingsCommand { get; }

    public ObservableCollection<FileTreeNode> WorkspaceFiles { get; } = new ObservableCollection<FileTreeNode>();
    public ObservableCollection<SearchResult> SearchResults { get; } = new ObservableCollection<SearchResult>();
    public ObservableCollection<GitChange> GitChanges { get; } = new ObservableCollection<GitChange>();

    private SidebarViewType _activeSidebarView = SidebarViewType.Explorer;
    public SidebarViewType ActiveSidebarView
    {
        get => _activeSidebarView;
        set => SetProperty(ref _activeSidebarView, value);
    }

    private bool _isAiPanelVisible;
    public bool IsAiPanelVisible
    {
        get => _isAiPanelVisible;
        set => SetProperty(ref _isAiPanelVisible, value);
    }

    private bool _isExtensionsPanelVisible;
    public bool IsExtensionsPanelVisible
    {
        get => _isExtensionsPanelVisible;
        set => SetProperty(ref _isExtensionsPanelVisible, value);
    }

    private string _chatInputText = "";
    public string ChatInputText
    {
        get => _chatInputText;
        set
        {
            _chatInputText = value;
            OnPropertyChanged(nameof(ChatInputText));
        }
    }

    private string _terminalOutput = "";
    public string TerminalOutput
    {
        get => _terminalOutput;
        set
        {
            _terminalOutput = value;
            OnPropertyChanged(nameof(TerminalOutput));
        }
    }

    private string _terminalInput = string.Empty;
    public string TerminalInput
    {
        get => _terminalInput;
        set => SetProperty(ref _terminalInput, value);
    }

    private string _searchQuery = string.Empty;
    public string SearchQuery
    {
        get => _searchQuery;
        set => SetProperty(ref _searchQuery, value);
    }

    private string _commitMessage = string.Empty;
    public string CommitMessage
    {
        get => _commitMessage;
        set => SetProperty(ref _commitMessage, value);
    }

    private string _runConfiguration = "dotnet run";
    public string RunConfiguration
    {
        get => _runConfiguration;
        set => SetProperty(ref _runConfiguration, value);
    }

    private bool _isTerminalVisible = true;
    public bool IsTerminalVisible
    {
        get => _isTerminalVisible;
        set
        {
            _isTerminalVisible = value;
            OnPropertyChanged(nameof(IsTerminalVisible));
        }
    }

    public ObservableCollection<ChatMessage> ChatMessages { get; } = new ObservableCollection<ChatMessage>();

    public Action<string>? OnTerminalCommandSent { get; set; }

    public ObservableCollection<NcIdeNative.ExtensionMarketplace.Api.IExtension> Extensions { get; } = new ObservableCollection<NcIdeNative.ExtensionMarketplace.Api.IExtension>();

    public MainViewModel()
    {
        _fileManager = new FileManager();
        _workspaceManager = new WorkspaceManager();
        _terminalService = new TerminalService();
        _aiAssistant = new AiAssistant();
        _searchService = new SearchService();
        _gitService = new GitService();
        _executionService = new ExecutionService();
        _settingsManager = new SettingsManager();
        
        OpenFileCommand = new RelayCommand(_ => OpenFile());
        NewFileCommand = new RelayCommand(_ => NewFile());
        SaveFileCommand = new RelayCommand(_ => SaveFile());
        CloseTabCommand = new RelayCommand(tab => CloseTab((EditorTab)tab!));
        CloseCurrentTabCommand = new RelayCommand(_ => CloseCurrentTab());
        ExitCommand = new RelayCommand(_ => System.Windows.Application.Current.Shutdown());
        OpenFolderCommand = new RelayCommand(_ => OpenFolder());
        AddNewFileCommand = new RelayCommand(node => AddNewFile((FileTreeNode)node!));
        AddNewFolderCommand = new RelayCommand(node => AddNewFolder((FileTreeNode)node!));
        DeleteFileCommand = new RelayCommand(node => DeleteFile((FileTreeNode)node!));
        ToggleThemeCommand = new RelayCommand(_ => { /* _themeManager.ToggleTheme(); */ });
        SetSidebarViewCommand = new RelayCommand(viewType => 
        {
            if (viewType is string viewTypeStr && Enum.TryParse<SidebarViewType>(viewTypeStr, out var parsedViewType))
            {
                ActiveSidebarView = parsedViewType;
            }
        });
        ToggleAiPanelCommand = new RelayCommand(_ => { 
            IsAiPanelVisible = !IsAiPanelVisible; 
            if(IsAiPanelVisible) IsExtensionsPanelVisible = false; 
        });
        SendAiMessageCommand = new RelayCommand(_ => _ = SendAiMessageAsync());
        ToggleTerminalCommand = new RelayCommand(_ => IsTerminalVisible = !IsTerminalVisible);
        SendTerminalCommand = new RelayCommand(_ => {
            OnTerminalCommandSent?.Invoke(TerminalInput);
            TerminalInput = "";
        });
        ToggleExtensionsPanelCommand = new RelayCommand(_ => {
            IsExtensionsPanelVisible = !IsExtensionsPanelVisible;
            if(IsExtensionsPanelVisible) IsAiPanelVisible = false;
        });
        ExecuteSearchCommand = new RelayCommand(async _ => await ExecuteSearch());
        RefreshGitCommand = new RelayCommand(async _ => await RefreshGitChanges());
        CommitGitCommand = new RelayCommand(async _ => await CommitGitChanges());
        RunProjectCommand = new RelayCommand(_ => RunProject());
        OpenSettingsCommand = new RelayCommand(_ => OpenFile(_settingsManager.GetSettingsFilePath()));
    }

    public void OpenFile()
    {
        var path = _fileManager.OpenFileDialog();
        if (path != null)
        {
            OpenFile(path);
        }
    }

    private void NewFile()
    {
        var tab = new EditorTab
        {
            FilePath = "Untitled",
            Content = "",
            IsModified = true
        };
        OpenTabs.Add(tab);
        SelectedTab = tab;
    }

    public void OpenFile(string path)
    {
        var existingTab = OpenTabs.FirstOrDefault(t => t.FilePath == path);
        if (existingTab != null)
        {
            SelectedTab = existingTab;
            return;
        }

        var tab = new EditorTab
        {
            FilePath = path,
            Content = _fileManager.ReadFile(path)
        };
        OpenTabs.Add(tab);
        SelectedTab = tab;
    }

    private void OpenFolder()
    {
        var path = _fileManager.OpenFolderDialog();
        if (path != null)
        {
            _currentWorkspacePath = path;
            WorkspaceFiles.Clear();
            var nodes = _workspaceManager.LoadWorkspace(path);
            foreach (var node in nodes)
            {
                WorkspaceFiles.Add(node);
            }
        }
    }

    private void AddNewFile(FileTreeNode node)
    {
        if (node == null) return;
        string dir = node.IsDirectory ? node.FullPath : System.IO.Path.GetDirectoryName(node.FullPath) ?? _currentWorkspacePath ?? "";
        if (string.IsNullOrEmpty(dir)) return;

        string path = System.IO.Path.Combine(dir, $"NewFile_{Guid.NewGuid().ToString().Substring(0,4)}.txt");
        System.IO.File.WriteAllText(path, "");
        RefreshWorkspace();
    }

    private void AddNewFolder(FileTreeNode node)
    {
        if (node == null) return;
        string dir = node.IsDirectory ? node.FullPath : System.IO.Path.GetDirectoryName(node.FullPath) ?? _currentWorkspacePath ?? "";
        if (string.IsNullOrEmpty(dir)) return;

        string path = System.IO.Path.Combine(dir, $"NewFolder_{Guid.NewGuid().ToString().Substring(0,4)}");
        System.IO.Directory.CreateDirectory(path);
        RefreshWorkspace();
    }

    private void DeleteFile(FileTreeNode node)
    {
        if (node == null || node.FullPath == _currentWorkspacePath) return;
        
        if (node.IsDirectory)
        {
            System.IO.Directory.Delete(node.FullPath, true);
        }
        else
        {
            System.IO.File.Delete(node.FullPath);
        }
        RefreshWorkspace();
    }

    private void RefreshWorkspace()
    {
        if (_currentWorkspacePath != null)
        {
            WorkspaceFiles.Clear();
            var nodes = _workspaceManager.LoadWorkspace(_currentWorkspacePath);
            foreach (var n in nodes) WorkspaceFiles.Add(n);
        }
    }

    private void SaveFile()
    {
        if (SelectedTab == null) return;

        if (string.IsNullOrEmpty(SelectedTab.FilePath))
        {
            var path = _fileManager.SaveFileDialog();
            if (path != null)
            {
                SelectedTab.FilePath = path;
            }
            else
            {
                return;
            }
        }
        _fileManager.SaveFile(SelectedTab.FilePath, SelectedTab.Content);
        SelectedTab.IsModified = false;
    }

    private async Task ExecuteSearch()
    {
        if (string.IsNullOrWhiteSpace(SearchQuery) || string.IsNullOrEmpty(_currentWorkspacePath))
            return;

        SearchResults.Clear();
        var results = await _searchService.SearchAsync(SearchQuery, _currentWorkspacePath);
        foreach (var result in results)
        {
            SearchResults.Add(result);
        }
    }

    private async Task RefreshGitChanges()
    {
        if (string.IsNullOrEmpty(_currentWorkspacePath)) return;
        
        var changes = await _gitService.GetChangesAsync(_currentWorkspacePath);
        GitChanges.Clear();
        foreach(var c in changes)
        {
            GitChanges.Add(c);
        }
    }

    private async Task CommitGitChanges()
    {
        if (string.IsNullOrEmpty(_currentWorkspacePath) || string.IsNullOrWhiteSpace(CommitMessage)) return;

        bool success = await _gitService.CommitAsync(_currentWorkspacePath, CommitMessage);
        if (success)
        {
            CommitMessage = string.Empty;
            await RefreshGitChanges();
        }
    }

    private void RunProject()
    {
        if (string.IsNullOrEmpty(_currentWorkspacePath) || string.IsNullOrWhiteSpace(RunConfiguration)) return;
        
        IsTerminalVisible = true;
        _terminalService.WriteCommand($"Running: {RunConfiguration}");

        _executionService.RunCommand(
            _currentWorkspacePath, 
            RunConfiguration,
            (output) => 
            {
                System.Windows.Application.Current.Dispatcher.Invoke(() => 
                {
                    TerminalOutput += output + "\n";
                });
            },
            () => 
            {
                System.Windows.Application.Current.Dispatcher.Invoke(() => 
                {
                    TerminalOutput += "\n--- Process Exited ---\n";
                });
            }
        );
    }

    public void CloseTab(EditorTab tab)
    {
        if (tab != null)
        {
            OpenTabs.Remove(tab);
            if (SelectedTab == tab)
            {
                SelectedTab = OpenTabs.FirstOrDefault();
            }
        }
    }

    private void CloseCurrentTab()
    {
        if (SelectedTab != null)
        {
            CloseTab(SelectedTab);
        }
    }

    private async Task SendAiMessageAsync()
    {
        if (string.IsNullOrWhiteSpace(ChatInputText)) return;

        var userMessage = ChatInputText;
        ChatInputText = string.Empty; 

        ChatMessages.Add(new ChatMessage { Sender = "You", Content = userMessage, IsAi = false });

        var aiResponse = await _aiAssistant.GenerateContentAsync(userMessage, SelectedTab?.Content ?? "");

        ChatMessages.Add(new ChatMessage { Sender = "Gemini ✨", Content = aiResponse, IsAi = true });
    }
}
