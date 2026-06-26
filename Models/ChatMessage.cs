namespace NcIdeNative.Models
{
    public class ChatMessage
    {
        public string Sender { get; set; } = string.Empty;
        public string Content { get; set; } = string.Empty;
        public bool IsAi { get; set; }
    }
}
