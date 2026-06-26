using System;
using System.Windows;

namespace NcIdeNative.Core;

public class ThemeManager
{
    private bool _isDarkTheme = true;

    public void ToggleTheme()
    {
        _isDarkTheme = !_isDarkTheme;
        ApplyTheme();
    }

    public void ApplyTheme()
    {
        var app = Application.Current;
        if (app == null) return;

        var themeUri = _isDarkTheme
            ? new Uri("pack://application:,,,/NcIdeNative;component/UI/Themes/DarkTheme.xaml")
            : new Uri("pack://application:,,,/NcIdeNative;component/UI/Themes/LightTheme.xaml");

        var newThemeDict = new ResourceDictionary { Source = themeUri };

        // Remove old theme dictionary and add new one
        app.Resources.MergedDictionaries.Clear();
        app.Resources.MergedDictionaries.Add(newThemeDict);
    }
}
