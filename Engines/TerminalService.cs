using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;

namespace NcIdeNative.Engines
{
    public class TerminalService
    {
        private Process? _process;
        private Action<string>? _onOutputReceived;

        public void Start(Action<string> onOutputReceived)
        {
            _onOutputReceived = onOutputReceived;

            try
            {
                var startInfo = new ProcessStartInfo
                {
                    FileName = "powershell.exe",
                    Arguments = "-NoExit -Command \"$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::UTF8\"",
                    RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    StandardErrorEncoding = Encoding.UTF8
                };

                _process = new Process { StartInfo = startInfo };
                
                _process.OutputDataReceived += (sender, e) => 
                {
                    if (e.Data != null)
                    {
                        _onOutputReceived?.Invoke(e.Data + Environment.NewLine);
                    }
                };

                _process.ErrorDataReceived += (sender, e) => 
                {
                    if (e.Data != null)
                    {
                        _onOutputReceived?.Invoke(e.Data + Environment.NewLine);
                    }
                };

                _process.Start();
                _process.BeginOutputReadLine();
                _process.BeginErrorReadLine();
            }
            catch (Exception ex)
            {
                _onOutputReceived?.Invoke($"Error starting terminal: {ex.Message}{Environment.NewLine}");
            }
        }

        public void WriteCommand(string command)
        {
            if (_process != null && !_process.HasExited)
            {
                _onOutputReceived?.Invoke($"{Environment.CurrentDirectory}> {command}{Environment.NewLine}");
                _process.StandardInput.WriteLine(command);
                _process.StandardInput.Flush();
            }
        }

        public void Stop()
        {
            if (_process != null && !_process.HasExited)
            {
                _process.StandardInput.WriteLine("exit");
                _process.WaitForExit(1000);
                if (!_process.HasExited)
                {
                    _process.Kill();
                }
            }
        }
    }
}
