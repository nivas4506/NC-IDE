using System;
using System.Collections.ObjectModel;
using System.IO;
using NcIdeNative.UI.ViewModels;
using NcIdeNative.Core;

namespace NcIdeNative.Models;

public class FileTreeNode : ObservableObject
{
    public string Name { get; set; } = string.Empty;
    public string FullPath { get; set; } = string.Empty;
    public bool IsDirectory { get; set; }
    public bool IsDummy { get; set; }
    
    public Action<FileTreeNode>? OnExpanded { get; set; }

    private bool _isExpanded;
    public bool IsExpanded
    {
        get => _isExpanded;
        set
        {
            if (SetProperty(ref _isExpanded, value))
            {
                if (_isExpanded && OnExpanded != null)
                {
                    OnExpanded(this);
                }
            }
        }
    }
    
    public ObservableCollection<FileTreeNode> Children { get; set; } = new ObservableCollection<FileTreeNode>();

    public FileTreeNode(string path, bool isDirectory, bool isDummy = false)
    {
        FullPath = path;
        Name = Path.GetFileName(path);
        if (string.IsNullOrEmpty(Name)) Name = path;
        IsDirectory = isDirectory;
        IsDummy = isDummy;
    }
}
