namespace NcIdeNative.ExtensionMarketplace.Api
{
    public interface IExtension
    {
        string Name { get; }
        string Version { get; }
        string Description { get; }
        void Initialize(IExtensionContext context);
    }
}
