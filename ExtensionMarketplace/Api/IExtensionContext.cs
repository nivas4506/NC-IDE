namespace NcIdeNative.ExtensionMarketplace.Api
{
    public interface IExtensionContext
    {
        void ShowMessage(string message);
        void RegisterCommand(string commandId, System.Action execute);
    }
}
