using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using NcIdeNative.ExtensionMarketplace.Api;

namespace NcIdeNative.ExtensionMarketplace
{
    public class PluginLoader : IExtensionContext
    {
        public List<IExtension> LoadedExtensions { get; } = new List<IExtension>();
        private readonly Action<string> _showMessageAction;

        public PluginLoader(Action<string> showMessageAction)
        {
            _showMessageAction = showMessageAction;
        }

        public void LoadExtensions()
        {
            var extPath = Path.Combine(Environment.CurrentDirectory, "Extensions");
            if (!Directory.Exists(extPath))
            {
                Directory.CreateDirectory(extPath);
            }

            foreach (var dll in Directory.GetFiles(extPath, "*.dll"))
            {
                try
                {
                    var assembly = Assembly.LoadFrom(dll);
                    var extensionTypes = assembly.GetTypes()
                        .Where(t => typeof(IExtension).IsAssignableFrom(t) && !t.IsInterface && !t.IsAbstract);

                    foreach (var type in extensionTypes)
                    {
                        if (Activator.CreateInstance(type) is IExtension extension)
                        {
                            extension.Initialize(this);
                            LoadedExtensions.Add(extension);
                        }
                    }
                }
                catch (Exception ex)
                {
                    Debug.WriteLine($"Failed to load extension {dll}: {ex.Message}");
                }
            }
        }

        public void ShowMessage(string message)
        {
            _showMessageAction?.Invoke(message);
        }

        public void RegisterCommand(string commandId, Action execute)
        {
            // Simple mapping for now
            Debug.WriteLine($"Command Registered: {commandId}");
        }
    }
}
