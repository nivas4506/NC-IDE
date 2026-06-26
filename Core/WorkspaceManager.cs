using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using NcIdeNative.Models;

namespace NcIdeNative.Core;

public class WorkspaceManager
{
    public ObservableCollection<FileTreeNode> LoadWorkspace(string rootPath)
    {
        var rootNodes = new ObservableCollection<FileTreeNode>();

        if (Directory.Exists(rootPath))
        {
            var rootNode = new FileTreeNode(rootPath, true) { Name = Path.GetFileName(rootPath) };
            if (string.IsNullOrEmpty(rootNode.Name))
            {
                rootNode.Name = rootPath; // Fallback for root drives like C:\
            }
            rootNode.OnExpanded = NodeExpanded;
            // Add a dummy node so it is expandable in UI
            rootNode.Children.Add(new FileTreeNode("Dummy", true, true));
            
            rootNodes.Add(rootNode);
            // Optionally auto-expand root
            rootNode.IsExpanded = true;
        }

        return rootNodes;
    }

    private void NodeExpanded(FileTreeNode node)
    {
        if (node.Children.Count == 1 && node.Children[0].IsDummy)
        {
            LoadDirectory(node);
        }
    }

    private void LoadDirectory(FileTreeNode node)
    {
        if (!node.IsDirectory) return;

        node.Children.Clear(); // Remove dummy

        try
        {
            var dirInfo = new DirectoryInfo(node.FullPath);
            
            // Directories first
            foreach (var dir in dirInfo.GetDirectories().OrderBy(d => d.Name))
            {
                // Skip hidden directories like .git
                if (dir.Attributes.HasFlag(FileAttributes.Hidden)) continue;

                var childNode = new FileTreeNode(dir.FullName, true);
                childNode.OnExpanded = NodeExpanded;
                childNode.Children.Add(new FileTreeNode("Dummy", true, true)); // Add dummy
                node.Children.Add(childNode);
            }

            // Files second
            foreach (var file in dirInfo.GetFiles().OrderBy(f => f.Name))
            {
                if (file.Attributes.HasFlag(FileAttributes.Hidden)) continue;

                var childNode = new FileTreeNode(file.FullName, false);
                node.Children.Add(childNode);
            }
        }
        catch (UnauthorizedAccessException)
        {
            // Ignore folders we don't have access to
        }
    }
}
