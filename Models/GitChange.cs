namespace NcIdeNative.Models;

public class GitChange
{
    public string Status { get; set; } = string.Empty;
    public string FilePath { get; set; } = string.Empty;
    
    // Status can be M (Modified), A (Added), D (Deleted), ?? (Untracked)
}
