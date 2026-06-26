// src/extensions/PluginLoader.h
#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonObject>
#include "ExtensionAPI.h"

struct PluginManifest {
    QString id;
    QString name;
    QString version;
    QString description;
    QString publisher;
    QString main; // Entry point file
    bool isValid = false;
};

class PluginLoader : public QObject {
    Q_OBJECT
public:
    explicit PluginLoader(ExtensionAPI* api, QObject* parent = nullptr);
    ~PluginLoader();

    QVector<PluginManifest> scanPlugins(const QString& directory);
    bool loadPlugin(const QString& pluginPath);
    void unloadPlugin(const QString& pluginId);

signals:
    void pluginLoaded(const QString& pluginId);
    void pluginUnloaded(const QString& pluginId);
    void pluginError(const QString& pluginId, const QString& error);

private:
    ExtensionAPI* m_api;
    QVector<QString> m_loadedPlugins;

    PluginManifest parseManifest(const QString& manifestPath);
};
