using NcIdeNative.Core;
using NcIdeNative.UI.ViewModels;

namespace NcIdeNative.Models;

public class EditorTab : ObservableObject
{
    private string _filePath = string.Empty;
    public string FilePath
    {
        get => _filePath;
        set
        {
            SetProperty(ref _filePath, value);
            OnPropertyChanged(nameof(FileName));
        }
    }

    public string FileName => string.IsNullOrEmpty(FilePath) ? "Untitled" : System.IO.Path.GetFileName(FilePath);

    private string _content = string.Empty;
    public string Content
    {
        get => _content;
        set => SetProperty(ref _content, value);
    }

    private bool _isModified;
    public bool IsModified
    {
        get => _isModified;
        set => SetProperty(ref _isModified, value);
    }
}
