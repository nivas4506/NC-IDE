// src/extensions/ExtensionManager.h
#pragma once
#include <QObject>
#include <QVector>
#include <QMap>
#include "PluginLoader.h"
#include "ExtensionAPI.h"

struct ExtensionInfo {
    QString id;
    QString name;
    QString version;
    QString description;
    QString publisher;
    bool enabled = true;
};

class ExtensionManager : public QObject, public ExtensionAPI {
    Q_OBJECT
public:
    explicit ExtensionManager(QObject* parent = nullptr);

    QVector<ExtensionInfo> extensions() const { return m_extensions; }
    void scanExtensions();
    void setExtensionEnabled(const QString& id, bool enabled);
    bool isExtensionEnabled(const QString& id) const;
    void installMockExtension(const QString& id, const QString& name, const QString& desc, const QString& pub);

    // ExtensionAPI Implementation
    void registerCommand(const QString& id, const QString& label, std::function<void()> callback) override;
    void registerSidebarView(const QString& id, const QString& title, QWidget* widget) override;
    void showNotification(const QString& message, const QString& type = "info") override;
    CodeEditor* getActiveEditor() override;
    QString getWorkspaceRoot() override;

signals:
    void extensionsChanged();

private:
    QVector<ExtensionInfo> m_extensions;
    QString m_extensionsPath;
    PluginLoader* m_pluginLoader;

    void loadInstalledExtensions();
    void createDefaultExtensions();
};
