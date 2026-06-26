// src/settings/SettingsManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSettings>

class SettingsManager : public QObject {
    Q_OBJECT
public:
    static SettingsManager& instance(); // singleton

    // Theme
    QString theme() const;                  // "dark" | "light"
    void setTheme(const QString& theme);

    // Font
    QString fontFamily() const;
    int fontSize() const;
    void setFontFamily(const QString& family);
    void setFontSize(int size);

    // Auto-save
    bool autoSaveEnabled() const;
    int autoSaveIntervalSeconds() const;     // configurable interval
    void setAutoSaveEnabled(bool enabled);
    void setAutoSaveIntervalSeconds(int seconds);

    // Recent files / projects (most-recent-first, capped at N entries)
    QStringList recentFiles() const;
    QStringList recentProjects() const;
    void addRecentFile(const QString& path);
    void addRecentProject(const QString& path);
    void clearRecentFiles();

    static constexpr int MAX_RECENT_ENTRIES = 15;

    // AI Integration
    QString aiApiKey() const;
    void setAiApiKey(const QString& key);

signals:
    void themeChanged(const QString& newTheme);
    void fontChanged(const QString& family, int size);
    void autoSaveSettingsChanged(bool enabled, int intervalSeconds);

private:
    SettingsManager();
    QSettings m_settings;

    void trimToMax(QStringList& list) const;
};
