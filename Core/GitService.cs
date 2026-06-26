using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using NcIdeNative.Models;

namespace NcIdeNative.Core;

public class GitService
{
    public async Task<List<GitChange>> GetChangesAsync(string repositoryPath)
    {
        var changes = new List<GitChange>();
        if (string.IsNullOrEmpty(repositoryPath) || !Directory.Exists(Path.Combine(repositoryPath, ".git")))
        {
            return changes; // Not a git repo
        }

        var output = await RunGitCommandAsync(repositoryPath, "status -s");
        var lines = output.Split('\n', System.StringSplitOptions.RemoveEmptyEntries);

        foreach (var line in lines)
        {
            if (line.Length > 3)
            {
                changes.Add(new GitChange
                {
                    Status = line.Substring(0, 2).Trim(),
                    FilePath = line.Substring(3).Trim()
                });
            }
        }
        return changes;
    }

    public async Task<bool> CommitAsync(string repositoryPath, string message)
    {
        if (string.IsNullOrEmpty(repositoryPath)) return false;

        // Stage all changes
        await RunGitCommandAsync(repositoryPath, "add .");

        // Commit
        var output = await RunGitCommandAsync(repositoryPath, $"commit -m \"{message}\"");
        return !output.Contains("nothing to commit");
    }

    private Task<string> RunGitCommandAsync(string repoPath, string arguments)
    {
        var tcs = new TaskCompletionSource<string>();

        var process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "git",
                Arguments = arguments,
                WorkingDirectory = repoPath,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            },
            EnableRaisingEvents = true
        };

        process.Exited += (sender, args) =>
        {
            var output = process.StandardOutput.ReadToEnd();
            var err = process.StandardError.ReadToEnd();
            process.Dispose();
            tcs.SetResult(output + err);
        };

        try
        {
            process.Start();
        }
        catch (System.ComponentModel.Win32Exception)
        {
            tcs.SetResult("Git not installed or not in PATH.");
        }

        return tcs.Task;
    }
}
