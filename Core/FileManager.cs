using System.IO;
using Microsoft.Win32;

namespace NcIdeNative.Core;

public class FileManager
{
    public string? OpenFileDialog()
    {
        var dialog = new Microsoft.Win32.OpenFileDialog();
        if (dialog.ShowDialog() == true)
        {
            return dialog.FileName;
        }
        return null;
    }

    public string? OpenFolderDialog()
    {
        var dialog = new Microsoft.Win32.OpenFolderDialog();
        if (dialog.ShowDialog() == true)
        {
            return dialog.FolderName;
        }
        return null;
    }

    public string? SaveFileDialog()
    {
        var saveFileDialog = new SaveFileDialog
        {
            Filter = "C++ Files (*.cpp;*.h;*.hpp)|*.cpp;*.h;*.hpp|All files (*.*)|*.*"
        };
        
        if (saveFileDialog.ShowDialog() == true)
        {
            return saveFileDialog.FileName;
        }
        return null;
    }

    public string ReadFile(string filePath)
    {
        return File.ReadAllText(filePath);
    }

    public void SaveFile(string filePath, string content)
    {
        File.WriteAllText(filePath, content);
    }
}
