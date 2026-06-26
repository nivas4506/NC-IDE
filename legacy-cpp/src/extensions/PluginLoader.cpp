// src/extensions/PluginLoader.cpp
#include "PluginLoader.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

PluginLoader::PluginLoader(ExtensionAPI* api, QObject* parent) : QObject(parent), m_api(api) {}

PluginLoader::~PluginLoader() {}

QVector<PluginManifest> PluginLoader::scanPlugins(const QString& directory) {
    QVector<PluginManifest> plugins;
    QDir dir(directory);
    if (!dir.exists()) return plugins;

    QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& subdir : subdirs) {
        QString manifestPath = dir.absoluteFilePath(subdir) + "/package.json";
        if (QFile::exists(manifestPath)) {
            PluginManifest manifest = parseManifest(manifestPath);
            if (manifest.isValid) {
                plugins.append(manifest);
            }
        }
    }
    return plugins;
}

PluginManifest PluginLoader::parseManifest(const QString& manifestPath) {
    PluginManifest manifest;
    QFile file(manifestPath);
    if (!file.open(QIODevice::ReadOnly)) return manifest;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        manifest.id = obj["name"].toString();
        manifest.name = obj["displayName"].toString(manifest.id);
        manifest.version = obj["version"].toString();
        manifest.description = obj["description"].toString();
        manifest.publisher = obj["publisher"].toString();
        manifest.main = obj["main"].toString();
        
        if (!manifest.id.isEmpty() && !manifest.version.isEmpty()) {
            manifest.isValid = true;
        }
    }
    return manifest;
}

bool PluginLoader::loadPlugin(const QString& pluginPath) {
    // Stub for loading a real plugin
    // In a full implementation, this would load a shared library or interpret a script
    QString manifestPath = pluginPath + "/package.json";
    PluginManifest manifest = parseManifest(manifestPath);
    if (manifest.isValid) {
        m_loadedPlugins.append(manifest.id);
        emit pluginLoaded(manifest.id);
        return true;
    }
    emit pluginError(manifestPath, "Invalid manifest");
    return false;
}

void PluginLoader::unloadPlugin(const QString& pluginId) {
    // Stub for unloading a plugin
    if (m_loadedPlugins.contains(pluginId)) {
        m_loadedPlugins.removeOne(pluginId);
        emit pluginUnloaded(pluginId);
    }
}
