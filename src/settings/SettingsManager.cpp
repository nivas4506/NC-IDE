// src/settings/SettingsManager.cpp
#include "SettingsManager.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager s_instance;
    return s_instance;
}

SettingsManager::SettingsManager()
    : m_settings("NC-IDE", "NC-IDE") {
}

QString SettingsManager::theme() const {
    return m_settings.value("general/theme", "dark").toString();
}

void SettingsManager::setTheme(const QString& theme) {
    if (this->theme() != theme) {
        m_settings.setValue("general/theme", theme);
        m_settings.sync();
        emit themeChanged(theme);
    }
}

QString SettingsManager::fontFamily() const {
#ifdef Q_OS_WIN
    return m_settings.value("general/fontFamily", "Consolas").toString();
#else
    return m_settings.value("general/fontFamily", "Monospace").toString();
#endif
}

int SettingsManager::fontSize() const {
    return m_settings.value("general/fontSize", 12).toInt();
}

void SettingsManager::setFontFamily(const QString& family) {
    if (this->fontFamily() != family) {
        m_settings.setValue("general/fontFamily", family);
        m_settings.sync();
        emit fontChanged(family, fontSize());
    }
}

void SettingsManager::setFontSize(int size) {
    if (this->fontSize() != size) {
        m_settings.setValue("general/fontSize", size);
        m_settings.sync();
        emit fontChanged(fontFamily(), size);
    }
}

bool SettingsManager::autoSaveEnabled() const {
    return m_settings.value("autosave/enabled", false).toBool();
}

int SettingsManager::autoSaveIntervalSeconds() const {
    return m_settings.value("autosave/intervalSeconds", 30).toInt();
}

void SettingsManager::setAutoSaveEnabled(bool enabled) {
    if (this->autoSaveEnabled() != enabled) {
        m_settings.setValue("autosave/enabled", enabled);
        m_settings.sync();
        emit autoSaveSettingsChanged(enabled, autoSaveIntervalSeconds());
    }
}

void SettingsManager::setAutoSaveIntervalSeconds(int seconds) {
    if (this->autoSaveIntervalSeconds() != seconds) {
        m_settings.setValue("autosave/intervalSeconds", seconds);
        m_settings.sync();
        emit autoSaveSettingsChanged(autoSaveEnabled(), seconds);
    }
}

QStringList SettingsManager::recentFiles() const {
    return m_settings.value("recent/files").toStringList();
}

QStringList SettingsManager::recentProjects() const {
    return m_settings.value("recent/projects").toStringList();
}

void SettingsManager::addRecentFile(const QString& path) {
    if (path.isEmpty()) return;
    QStringList files = recentFiles();
    files.removeAll(path);
    files.prepend(path);
    trimToMax(files);
    m_settings.setValue("recent/files", files);
    m_settings.sync();
}

void SettingsManager::addRecentProject(const QString& path) {
    if (path.isEmpty()) return;
    QStringList projects = recentProjects();
    projects.removeAll(path);
    projects.prepend(path);
    trimToMax(projects);
    m_settings.setValue("recent/projects", projects);
    m_settings.sync();
}

void SettingsManager::clearRecentFiles() {
    m_settings.setValue("recent/files", QStringList());
    m_settings.sync();
}

void SettingsManager::trimToMax(QStringList& list) const {
    while (list.size() > MAX_RECENT_ENTRIES) {
        list.removeLast();
    }
}
