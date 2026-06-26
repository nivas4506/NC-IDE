using System;
using System.IO;
using System.Text.Json;

namespace NcIdeNative.Core;

public class SettingsManager
{
    private readonly string _settingsFilePath;

    public SettingsManager()
    {
        var appData = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        var appFolder = Path.Combine(appData, "NCIDE");
        Directory.CreateDirectory(appFolder);
        _settingsFilePath = Path.Combine(appFolder, "settings.json");

        if (!File.Exists(_settingsFilePath))
        {
            var defaultSettings = @"{
    ""editor.fontSize"": 14,
    ""editor.fontFamily"": ""Consolas"",
    ""workbench.theme"": ""Dark"",
    ""terminal.integrated.shell"": ""cmd.exe""
}";
            File.WriteAllText(_settingsFilePath, defaultSettings);
        }
    }

    public string GetSettingsFilePath()
    {
        return _settingsFilePath;
    }
}
