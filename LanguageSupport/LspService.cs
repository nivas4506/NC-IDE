using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace NcIdeNative.LanguageSupport
{
    public class LspPosition
    {
        public int Line { get; set; }
        public int Character { get; set; }
    }

    public class LspRange
    {
        public LspPosition Start { get; set; } = new LspPosition();
        public LspPosition End { get; set; } = new LspPosition();
    }

    public class LspDiagnostic
    {
        public LspRange Range { get; set; } = new LspRange();
        public int Severity { get; set; }
        public string Message { get; set; } = "";
    }

    public class LspLocation
    {
        public string Uri { get; set; } = "";
        public LspRange Range { get; set; } = new LspRange();
    }

    public class LspService
    {
        public event Action<string, List<LspDiagnostic>>? OnDiagnosticsReceived;
        private Process? _clangdProcess;
        private StreamWriter? _stdin;
        private StreamReader? _stdout;
        private int _messageId = 1;
        private readonly Dictionary<int, TaskCompletionSource<JsonElement>> _pendingRequests = new();
        private readonly LanguageServerConfig _config;

        public LspService(LanguageServerConfig config)
        {
            _config = config;
        }

        public async Task StartAsync()
        {
            try
            {
                var startInfo = new ProcessStartInfo
                {
                    FileName = _config.ExecutablePath,
                    Arguments = _config.Arguments,
                    UseShellExecute = false,
                    RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    StandardInputEncoding = Encoding.UTF8
                };

                _clangdProcess = Process.Start(startInfo);
                if (_clangdProcess != null)
                {
                    _stdin = _clangdProcess.StandardInput;
                    _stdout = _clangdProcess.StandardOutput;
                    _ = Task.Run(ReadLoop);

                    // Send initialize
                    var initParams = new
                    {
                        processId = Environment.ProcessId,
                        rootUri = (string?)null,
                        capabilities = new { }
                    };
                    await SendRequestAsync("initialize", initParams);
                    SendNotification("initialized", new { });
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Failed to start {_config.ExecutablePath}: {ex.Message}");
            }
        }

        private void SendNotification(string method, object @params)
        {
            if (_stdin == null) return;
            var message = new
            {
                jsonrpc = "2.0",
                method = method,
                @params = @params
            };
            WriteMessage(message);
        }

        private Task<JsonElement> SendRequestAsync(string method, object @params)
        {
            if (_stdin == null) return Task.FromResult(default(JsonElement));
            
            int id = _messageId++;
            var tcs = new TaskCompletionSource<JsonElement>();
            _pendingRequests[id] = tcs;

            var message = new
            {
                jsonrpc = "2.0",
                id = id,
                method = method,
                @params = @params
            };

            WriteMessage(message);
            return tcs.Task;
        }

        private void WriteMessage(object message)
        {
            var json = JsonSerializer.Serialize(message);
            var utf8Bytes = Encoding.UTF8.GetBytes(json);
            _stdin?.Write($"Content-Length: {utf8Bytes.Length}\r\n\r\n{json}");
            _stdin?.Flush();
        }

        private async Task ReadLoop()
        {
            if (_stdout == null) return;
            try
            {
                while (true)
                {
                    var line = await _stdout.ReadLineAsync();
                    if (line == null) break; // Stream ended
                    if (string.IsNullOrEmpty(line)) continue;

                    if (line.StartsWith("Content-Length: "))
                    {
                        if (int.TryParse(line.Substring(16).Trim(), out int length))
                        {
                            await _stdout.ReadLineAsync(); // Skip empty line
                            var buffer = new char[length];
                            int read = 0;
                            while (read < length)
                            {
                                read += await _stdout.ReadAsync(buffer, read, length - read);
                            }

                            var json = new string(buffer);
                            ProcessMessage(json);
                        }
                    }
                }
            }
            catch (Exception) { /* Handle disconnect */ }
        }

        private void ProcessMessage(string json)
        {
            try
            {
                using var doc = JsonDocument.Parse(json);
                var root = doc.RootElement;

                if (root.TryGetProperty("id", out var idElement) && idElement.ValueKind == JsonValueKind.Number)
                {
                    int id = idElement.GetInt32();
                    if (_pendingRequests.TryGetValue(id, out var tcs))
                    {
                        if (root.TryGetProperty("result", out var result))
                        {
                            tcs.SetResult(result.Clone());
                        }
                        else
                        {
                            tcs.SetResult(default);
                        }
                        _pendingRequests.Remove(id);
                    }
                }
                else if (root.TryGetProperty("method", out var methodElement))
                {
                    string? method = methodElement.GetString();
                    if (method == "textDocument/publishDiagnostics" && root.TryGetProperty("params", out var prms))
                    {
                        if (prms.TryGetProperty("uri", out var uriElement) && prms.TryGetProperty("diagnostics", out var diagnosticsElement))
                        {
                            string uri = uriElement.GetString() ?? "";
                            var diags = new List<LspDiagnostic>();
                            foreach (var d in diagnosticsElement.EnumerateArray())
                            {
                                var diagnostic = new LspDiagnostic();
                                if (d.TryGetProperty("message", out var msg)) diagnostic.Message = msg.GetString() ?? "";
                                if (d.TryGetProperty("severity", out var sev)) diagnostic.Severity = sev.GetInt32();
                                
                                if (d.TryGetProperty("range", out var rangeObj))
                                {
                                    if (rangeObj.TryGetProperty("start", out var startObj))
                                    {
                                        if (startObj.TryGetProperty("line", out var l)) diagnostic.Range.Start.Line = l.GetInt32();
                                        if (startObj.TryGetProperty("character", out var c)) diagnostic.Range.Start.Character = c.GetInt32();
                                    }
                                    if (rangeObj.TryGetProperty("end", out var endObj))
                                    {
                                        if (endObj.TryGetProperty("line", out var l)) diagnostic.Range.End.Line = l.GetInt32();
                                        if (endObj.TryGetProperty("character", out var c)) diagnostic.Range.End.Character = c.GetInt32();
                                    }
                                }
                                diags.Add(diagnostic);
                            }
                            OnDiagnosticsReceived?.Invoke(uri, diags);
                        }
                    }
                }
            }
            catch { }
        }

        public void DidOpen(string uri, string text)
        {
            SendNotification("textDocument/didOpen", new
            {
                textDocument = new
                {
                    uri = uri,
                    languageId = _config.LanguageId,
                    version = 1,
                    text = text
                }
            });
        }

        public void DidChange(string uri, string text, int version)
        {
            SendNotification("textDocument/didChange", new
            {
                textDocument = new { uri = uri, version = version },
                contentChanges = new[] { new { text = text } }
            });
        }

        public async Task<List<string>> GetCompletionsAsync(string uri, int line, int character)
        {
            var completions = new List<string>();
            var result = await SendRequestAsync("textDocument/completion", new
            {
                textDocument = new { uri = uri },
                position = new { line = line, character = character }
            });

            if (result.ValueKind == JsonValueKind.Object && result.TryGetProperty("items", out var items))
            {
                foreach (var item in items.EnumerateArray())
                {
                    if (item.TryGetProperty("label", out var label))
                    {
                        completions.Add(label.GetString() ?? "");
                    }
                }
            }
            else if (result.ValueKind == JsonValueKind.Array)
            {
                foreach (var item in result.EnumerateArray())
                {
                    if (item.TryGetProperty("label", out var label))
                    {
                        completions.Add(label.GetString() ?? "");
                    }
                }
            }
            return completions;
        }

        public async Task<string> GetHoverInfoAsync(string uri, int line, int character)
        {
            var result = await SendRequestAsync("textDocument/hover", new
            {
                textDocument = new { uri = uri },
                position = new { line = line, character = character }
            });

            if (result.ValueKind == JsonValueKind.Object && result.TryGetProperty("contents", out var contents))
            {
                if (contents.ValueKind == JsonValueKind.Object && contents.TryGetProperty("value", out var valueStr))
                {
                    return valueStr.GetString() ?? "";
                }
                else if (contents.ValueKind == JsonValueKind.String)
                {
                    return contents.GetString() ?? "";
                }
            }
            return "";
        }

        public async Task<LspLocation?> GetDefinitionAsync(string uri, int line, int character)
        {
            var result = await SendRequestAsync("textDocument/definition", new
            {
                textDocument = new { uri = uri },
                position = new { line = line, character = character }
            });

            JsonElement locationElement = default;
            if (result.ValueKind == JsonValueKind.Array && result.GetArrayLength() > 0)
            {
                locationElement = result[0];
            }
            else if (result.ValueKind == JsonValueKind.Object)
            {
                locationElement = result;
            }

            if (locationElement.ValueKind == JsonValueKind.Object)
            {
                var loc = new LspLocation();
                if (locationElement.TryGetProperty("uri", out var uriObj) || locationElement.TryGetProperty("targetUri", out uriObj))
                {
                    loc.Uri = uriObj.GetString() ?? "";
                }
                if (locationElement.TryGetProperty("range", out var rangeObj) || locationElement.TryGetProperty("targetSelectionRange", out rangeObj))
                {
                    if (rangeObj.TryGetProperty("start", out var startObj))
                    {
                        if (startObj.TryGetProperty("line", out var l)) loc.Range.Start.Line = l.GetInt32();
                        if (startObj.TryGetProperty("character", out var c)) loc.Range.Start.Character = c.GetInt32();
                    }
                    if (rangeObj.TryGetProperty("end", out var endObj))
                    {
                        if (endObj.TryGetProperty("line", out var l)) loc.Range.End.Line = l.GetInt32();
                        if (endObj.TryGetProperty("character", out var c)) loc.Range.End.Character = c.GetInt32();
                    }
                }
                return loc;
            }
            return null;
        }

        public void Stop()
        {
            try
            {
                SendNotification("exit", new { });
                _clangdProcess?.Kill();
            }
            catch { }
        }
    }
}
