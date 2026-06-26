namespace NcIdeNative.Models;

public class SearchResult
{
    public string FilePath { get; set; } = string.Empty;
    public string FileName { get; set; } = string.Empty;
    public int LineNumber { get; set; }
    public string MatchLine { get; set; } = string.Empty;
}
