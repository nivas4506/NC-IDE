// src/services/ServiceLocator.h
#pragma once

class GitManager;
class BuildManager;
class RunManager;
class ExtensionManager;
class CommandRegistry;
class SettingsManager;
class WorkspaceManager;
class FileManager;
class ThemeManager;
class NotificationManager;
class NetworkManager;

class ServiceLocator {
public:
    static GitManager* gitManager() { return m_gitManager; }
    static void registerGitManager(GitManager* service) { m_gitManager = service; }

    static BuildManager* buildManager() { return m_buildManager; }
    static void registerBuildManager(BuildManager* service) { m_buildManager = service; }

    static RunManager* runManager() { return m_runManager; }
    static void registerRunManager(RunManager* service) { m_runManager = service; }

    static ExtensionManager* extensionManager() { return m_extensionManager; }
    static void registerExtensionManager(ExtensionManager* service) { m_extensionManager = service; }

    static CommandRegistry* commandRegistry() { return m_commandRegistry; }
    static void registerCommandRegistry(CommandRegistry* service) { m_commandRegistry = service; }

    static SettingsManager* settingsManager() { return m_settingsManager; }
    static void registerSettingsManager(SettingsManager* service) { m_settingsManager = service; }

    static WorkspaceManager* workspaceManager() { return m_workspaceManager; }
    static void registerWorkspaceManager(WorkspaceManager* service) { m_workspaceManager = service; }

    static FileManager* fileManager() { return m_fileManager; }
    static void registerFileManager(FileManager* service) { m_fileManager = service; }

    static ThemeManager* themeManager() { return m_themeManager; }
    static void registerThemeManager(ThemeManager* service) { m_themeManager = service; }

    static NotificationManager* notificationManager() { return m_notificationManager; }
    static void registerNotificationManager(NotificationManager* service) { m_notificationManager = service; }

    static NetworkManager* networkManager() { return m_networkManager; }
    static void registerNetworkManager(NetworkManager* service) { m_networkManager = service; }

private:
    static inline GitManager* m_gitManager = nullptr;
    static inline BuildManager* m_buildManager = nullptr;
    static inline RunManager* m_runManager = nullptr;
    static inline ExtensionManager* m_extensionManager = nullptr;
    static inline CommandRegistry* m_commandRegistry = nullptr;
    static inline SettingsManager* m_settingsManager = nullptr;
    static inline WorkspaceManager* m_workspaceManager = nullptr;
    static inline FileManager* m_fileManager = nullptr;
    static inline ThemeManager* m_themeManager = nullptr;
    static inline NotificationManager* m_notificationManager = nullptr;
    static inline NetworkManager* m_networkManager = nullptr;
};
