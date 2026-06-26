using System.Collections.Generic;
using System.IO;

namespace NcIdeNative.LanguageSupport
{
    public class LanguageServerConfig
    {
        public string ExecutablePath { get; set; } = "";
        public string Arguments { get; set; } = "";
        public string LanguageId { get; set; } = "";
    }

    public static class LanguageServerRegistry
    {
        private static readonly Dictionary<string, LanguageServerConfig> _extensionsToServers = new(System.StringComparer.OrdinalIgnoreCase)
        {
            { ".cpp", new LanguageServerConfig { ExecutablePath = "clangd", Arguments = "--log=error", LanguageId = "cpp" } },
            { ".c", new LanguageServerConfig { ExecutablePath = "clangd", Arguments = "--log=error", LanguageId = "c" } },
            { ".h", new LanguageServerConfig { ExecutablePath = "clangd", Arguments = "--log=error", LanguageId = "cpp" } },
            { ".hpp", new LanguageServerConfig { ExecutablePath = "clangd", Arguments = "--log=error", LanguageId = "cpp" } },
            { ".cs", new LanguageServerConfig { ExecutablePath = "omnisharp", Arguments = "-lsp", LanguageId = "csharp" } },
            { ".py", new LanguageServerConfig { ExecutablePath = "pylsp", Arguments = "", LanguageId = "python" } },
            { ".ts", new LanguageServerConfig { ExecutablePath = "typescript-language-server", Arguments = "--stdio", LanguageId = "typescript" } },
            { ".js", new LanguageServerConfig { ExecutablePath = "typescript-language-server", Arguments = "--stdio", LanguageId = "javascript" } },
        };

        public static LanguageServerConfig? GetServerConfig(string filePath)
        {
            if (string.IsNullOrEmpty(filePath)) return null;
            string ext = Path.GetExtension(filePath);
            if (_extensionsToServers.TryGetValue(ext, out var config))
            {
                return config;
            }
            return null;
        }
    }
}
