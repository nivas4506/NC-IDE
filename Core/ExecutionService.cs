using System.Diagnostics;
using System.Threading.Tasks;

namespace NcIdeNative.Core;

public class ExecutionService
{
    private Process? _currentProcess;

    public void RunCommand(string workingDirectory, string command, System.Action<string> onOutput, System.Action onExit)
    {
        if (_currentProcess != null && !_currentProcess.HasExited)
        {
            _currentProcess.Kill();
        }

        var processInfo = new ProcessStartInfo
        {
            FileName = "cmd.exe",
            Arguments = $"/c {command}",
            WorkingDirectory = workingDirectory,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        _currentProcess = new Process { StartInfo = processInfo, EnableRaisingEvents = true };

        _currentProcess.OutputDataReceived += (s, e) => { if (e.Data != null) onOutput(e.Data); };
        _currentProcess.ErrorDataReceived += (s, e) => { if (e.Data != null) onOutput($"ERROR: {e.Data}"); };

        _currentProcess.Exited += (s, e) => {
            _currentProcess.Dispose();
            _currentProcess = null;
            onExit();
        };

        _currentProcess.Start();
        _currentProcess.BeginOutputReadLine();
        _currentProcess.BeginErrorReadLine();
    }
}
