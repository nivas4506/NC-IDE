using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.IO;
using ICSharpCode.AvalonEdit.CodeCompletion;
using ICSharpCode.AvalonEdit.Highlighting;
using NcIdeNative.UI.ViewModels;
using NcIdeNative.Engines;
using NcIdeNative.LanguageSupport;
using NcIdeNative.LanguageSupport.UI;
using NcIdeNative.ExtensionMarketplace;
using NcIdeNative.BackendServices;
using NcIdeNative.Models;

namespace NcIdeNative.UI.Views;

public partial class MainWindow : Window
{
    private MainViewModel _viewModel;
    private readonly Dictionary<string, LspService> _activeLspServices = new();
    private LspService? _currentLspService;
    private readonly TerminalService _terminalService;
    private readonly PluginLoader _PluginLoader;
    private CompletionWindow? _completionWindow;
    private ToolTip? _hoverToolTip;
    private TextMarkerService? _textMarkerService;
    private string _documentUri = "file:///temp.cpp";
    private int _documentVersion = 1;

    public MainWindow()
    {
        InitializeComponent();
        
        _terminalService = new TerminalService();
        _PluginLoader = new PluginLoader(msg => {
            Dispatcher.Invoke(() => {
                _viewModel.TerminalOutput += $"[Extension] {msg}\n";
            });
        });

        _viewModel = new MainViewModel();
        DataContext = _viewModel;

        Loaded += MainWindow_Loaded;
        Closed += MainWindow_Closed;
        
        textEditor.TextChanged += TextEditor_TextChanged;
        textEditor.TextArea.TextEntered += TextEditor_TextArea_TextEntered;
        textEditor.TextArea.TextEntering += TextEditor_TextArea_TextEntering;
        textEditor.TextArea.KeyDown += TextEditor_TextArea_KeyDown;
        textEditor.MouseHover += TextEditor_MouseHover;
        textEditor.MouseHoverStopped += TextEditor_MouseHoverStopped;

        _textMarkerService = new TextMarkerService(textEditor.Document);
        textEditor.TextArea.TextView.BackgroundRenderers.Add(_textMarkerService);
        textEditor.TextArea.TextView.LineTransformers.Add(_textMarkerService);

        // Initialize editor text
        if (_viewModel.SelectedTab != null)
            textEditor.Text = _viewModel.SelectedTab.Content;

        // Two-way binding workaround for AvalonEdit.Text
        textEditor.TextChanged += (s, e) => 
        {
            if (_viewModel.SelectedTab != null)
            {
                _viewModel.SelectedTab.Content = textEditor.Text;
                _viewModel.SelectedTab.IsModified = true;
            }
        };

        _viewModel.PropertyChanged += async (s, e) =>
        {
            if (e.PropertyName == nameof(MainViewModel.SelectedTab))
            {
                if (_viewModel.SelectedTab != null)
                {
                    if (textEditor.Text != _viewModel.SelectedTab.Content)
                    {
                        textEditor.Text = _viewModel.SelectedTab.Content;
                    }
                    var ext = System.IO.Path.GetExtension(_viewModel.SelectedTab.FileName);
                    var def = HighlightingManager.Instance.GetDefinitionByExtension(ext);
                    textEditor.SyntaxHighlighting = def;

                    _documentUri = "file:///" + _viewModel.SelectedTab.FilePath.Replace('\\', '/');
                    await SwitchLspServiceAsync(_viewModel.SelectedTab.FilePath);
                    _currentLspService?.DidOpen(_documentUri, textEditor.Text);
                }
                else
                {
                    _currentLspService = null;
                }
            }
        };

        _viewModel.OnTerminalCommandSent = (command) =>
        {
            _terminalService.WriteCommand(command);
        };
    }

    private async Task SwitchLspServiceAsync(string filePath)
    {
        var config = LanguageServerRegistry.GetServerConfig(filePath);
        if (config == null)
        {
            _currentLspService = null;
            return;
        }

        if (!_activeLspServices.TryGetValue(config.LanguageId, out var service))
        {
            service = new LspService(config);
            service.OnDiagnosticsReceived += LspService_OnDiagnosticsReceived;
            await service.StartAsync();
            _activeLspServices[config.LanguageId] = service;
        }

        _currentLspService = service;
    }

    private void LspService_OnDiagnosticsReceived(string uri, System.Collections.Generic.List<LspDiagnostic> diagnostics)
    {
        Dispatcher.Invoke(() =>
        {
            if (uri != _documentUri) return;
            
            _textMarkerService?.Clear();

            foreach (var diag in diagnostics)
            {
                try
                {
                    var startLine = textEditor.Document.GetLineByNumber(diag.Range.Start.Line + 1);
                    var endLine = textEditor.Document.GetLineByNumber(diag.Range.End.Line + 1);
                    int startOffset = startLine.Offset + diag.Range.Start.Character;
                    int endOffset = endLine.Offset + diag.Range.End.Character;
                    int length = endOffset - startOffset;

                    if (length > 0)
                    {
                        var marker = _textMarkerService?.Create(startOffset, length);
                        if (marker != null)
                        {
                            // Severity 1 = Error, 2 = Warning, 3 = Information, 4 = Hint
                            marker.MarkerColor = diag.Severity == 1 ? Colors.Red : Colors.Yellow;
                            marker.ToolTip = diag.Message;
                        }
                    }
                }
                catch { } // Ignore lines out of bounds
            }
        });
    }

    private void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
        _terminalService.Start(output =>
        {
            Dispatcher.Invoke(() =>
            {
                _viewModel.TerminalOutput += output;
                TerminalScrollViewer?.ScrollToBottom();
            });
        });

        _PluginLoader.LoadExtensions();
        foreach (var ext in _PluginLoader.LoadedExtensions)
        {
            _viewModel.Extensions.Add(ext);
        }
    }

    private void MainWindow_Closed(object? sender, EventArgs e)
    {
        foreach (var svc in _activeLspServices.Values)
        {
            svc.OnDiagnosticsReceived -= LspService_OnDiagnosticsReceived;
            svc.Stop();
        }
        _terminalService.Stop();
    }

    private void TextEditor_TextChanged(object? sender, EventArgs e)
    {
        _documentVersion++;
        _currentLspService?.DidChange(_documentUri, textEditor.Text, _documentVersion);
    }

    private async void TextEditor_TextArea_TextEntered(object sender, TextCompositionEventArgs e)
    {
        // Simple trigger on . or > or : or space
        if (e.Text == "." || e.Text == ">" || e.Text == ":" || e.Text == " ")
        {
            var loc = textEditor.Document.GetLocation(textEditor.CaretOffset);
            
            if (_currentLspService != null)
            {
                _completionWindow = new CompletionWindow(textEditor.TextArea);
                var completions = await _currentLspService.GetCompletionsAsync(_documentUri, loc.Line - 1, loc.Column - 1);
                foreach (var completion in completions)
                {
                    _completionWindow.CompletionList.CompletionData.Add(new NcIdeNative.LanguageSupport.UI.CompletionData(completion));
                }
                _completionWindow.Show();
                _completionWindow.Closed += delegate {
                    _completionWindow = null;
                };
            }
        }
    }

    private void TextEditor_TextArea_TextEntering(object sender, TextCompositionEventArgs e)
    {
        if (e.Text.Length > 0 && _completionWindow != null)
        {
            if (!char.IsLetterOrDigit(e.Text[0]))
            {
                // If completion window is open and user types non-alphanumeric, 
                // insert completion if requested
                _completionWindow.CompletionList.RequestInsertion(e);
            }
        }
    }

    private async void TextEditor_TextArea_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.Key == Key.F12 && _currentLspService != null)
        {
            var loc = textEditor.Document.GetLocation(textEditor.CaretOffset);
            var def = await _currentLspService.GetDefinitionAsync(_documentUri, loc.Line - 1, loc.Column - 1);
            if (def != null && !string.IsNullOrEmpty(def.Uri))
            {
                string filePath = def.Uri;
                if (filePath.StartsWith("file:///"))
                {
                    filePath = filePath.Substring(8).Replace('/', '\\');
                    // Handle URL encoded windows drive letters if necessary (like c%3A -> c:)
                    filePath = Uri.UnescapeDataString(filePath);
                }

                _viewModel.OpenFile(filePath);
                
                await Dispatcher.InvokeAsync(() => {
                    try {
                        var targetLine = textEditor.Document.GetLineByNumber(def.Range.Start.Line + 1);
                        textEditor.ScrollToLine(def.Range.Start.Line + 1);
                        textEditor.CaretOffset = targetLine.Offset + def.Range.Start.Character;
                    } catch {}
                }, System.Windows.Threading.DispatcherPriority.Background);
            }
            e.Handled = true;
        }
    }

    private async void TextEditor_MouseHover(object sender, MouseEventArgs e)
    {
        if (_currentLspService == null) return;
        var pos = textEditor.GetPositionFromPoint(e.GetPosition(textEditor));
        if (pos != null)
        {
            var hoverText = await _currentLspService.GetHoverInfoAsync(_documentUri, pos.Value.Line - 1, pos.Value.Column - 1);
            if (!string.IsNullOrEmpty(hoverText))
            {
                if (_hoverToolTip == null)
                {
                    _hoverToolTip = new ToolTip();
                    _hoverToolTip.PlacementTarget = textEditor;
                }
                _hoverToolTip.Content = hoverText;
                _hoverToolTip.IsOpen = true;
                e.Handled = true;
            }
        }
    }

    private void TextEditor_MouseHoverStopped(object sender, MouseEventArgs e)
    {
        if (_hoverToolTip != null)
        {
            _hoverToolTip.IsOpen = false;
        }
    }

    private void TitleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        if (e.ClickCount == 2)
        {
            MaximizeButton_Click(sender, e);
        }
        else
        {
            DragMove();
        }
    }

    private void CloseButton_Click(object sender, RoutedEventArgs e)
    {
        Close();
    }

    private void MaximizeButton_Click(object sender, RoutedEventArgs e)
    {
        WindowState = WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;
    }

    private void MinimizeButton_Click(object sender, RoutedEventArgs e)
    {
        WindowState = WindowState.Minimized;
    }

    private void TreeViewItem_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
        if (sender is TreeViewItem tvi && tvi.DataContext is FileTreeNode node && !node.IsDirectory)
        {
            if (DataContext is MainViewModel vm)
            {
                vm.OpenFile(node.FullPath);
            }
        }
    }

    private void SearchResults_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
        if (sender is ListBox listBox && listBox.SelectedItem is SearchResult result)
        {
            if (DataContext is MainViewModel vm)
            {
                vm.OpenFile(result.FilePath);
            }
        }
    }
}
