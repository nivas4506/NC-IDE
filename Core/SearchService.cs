using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using NcIdeNative.Models;

namespace NcIdeNative.Core;

public class SearchService
{
    public async Task<List<SearchResult>> SearchAsync(string query, string rootPath)
    {
        var results = new List<SearchResult>();
        if (string.IsNullOrWhiteSpace(query) || string.IsNullOrWhiteSpace(rootPath) || !Directory.Exists(rootPath))
        {
            return results;
        }

        try
        {
            var files = Directory.GetFiles(rootPath, "*.*", SearchOption.AllDirectories)
                                 .Where(f => !f.Contains(".git") && !f.Contains("\\bin\\") && !f.Contains("\\obj\\"));

            foreach (var file in files)
            {
                var lines = await File.ReadAllLinesAsync(file);
                for (int i = 0; i < lines.Length; i++)
                {
                    if (lines[i].Contains(query, StringComparison.OrdinalIgnoreCase))
                    {
                        results.Add(new SearchResult
                        {
                            FilePath = file,
                            FileName = Path.GetFileName(file),
                            LineNumber = i + 1,
                            MatchLine = lines[i].Trim()
                        });
                    }
                }
            }
        }
        catch (Exception)
        {
            // Ignore access errors
        }

        return results;
    }
}
