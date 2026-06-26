using System;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace NcIdeNative.BackendServices
{
    public class AiAssistant
    {
        private readonly HttpClient _httpClient;
        private readonly string _apiKey;
        private const string ModelName = "gemini-1.5-flash"; // Or gemini-2.5-flash

        public AiAssistant()
        {
            _httpClient = new HttpClient();
            _apiKey = Environment.GetEnvironmentVariable("GEMINI_API_KEY");
        }

        public async Task<string> GenerateContentAsync(string prompt, string context = "")
        {
            if (string.IsNullOrEmpty(_apiKey))
            {
                return "Error: GEMINI_API_KEY environment variable is not set. Please set it to use the AI assistant.";
            }

            var requestUri = $"https://generativelanguage.googleapis.com/v1beta/models/{ModelName}:generateContent?key={_apiKey}";

            var fullPrompt = prompt;
            if (!string.IsNullOrEmpty(context))
            {
                fullPrompt = $"Context (Current Code):\n```cpp\n{context}\n```\n\nUser Question:\n{prompt}";
            }

            var requestBody = new
            {
                contents = new[]
                {
                    new
                    {
                        parts = new[] { new { text = fullPrompt } }
                    }
                }
            };

            var jsonContent = new StringContent(JsonSerializer.Serialize(requestBody), Encoding.UTF8, "application/json");

            try
            {
                var response = await _httpClient.PostAsync(requestUri, jsonContent);
                response.EnsureSuccessStatusCode();

                var responseString = await response.Content.ReadAsStringAsync();
                
                // Parse the response
                using (var doc = JsonDocument.Parse(responseString))
                {
                    var root = doc.RootElement;
                    var candidates = root.GetProperty("candidates");
                    if (candidates.GetArrayLength() > 0)
                    {
                        var firstCandidate = candidates[0];
                        var content = firstCandidate.GetProperty("content");
                        var parts = content.GetProperty("parts");
                        if (parts.GetArrayLength() > 0)
                        {
                            return parts[0].GetProperty("text").GetString() ?? "No response generated.";
                        }
                    }
                }

                return "Error parsing AI response.";
            }
            catch (Exception ex)
            {
                return $"Error connecting to AI service: {ex.Message}";
            }
        }
    }
}
