// src/extensions/ExtensionManager.cpp
#include "ExtensionManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

ExtensionManager::ExtensionManager(QObject* parent) : QObject(parent) {
    m_extensionsPath = QDir::homePath() + "/.ncide/extensions";
    
    QDir dir;
    if (!dir.exists(m_extensionsPath)) {
        dir.mkpath(m_extensionsPath);
    }
    
    createDefaultExtensions();
    scanExtensions();
}

void ExtensionManager::createDefaultExtensions() {
    QMap<QString, QJsonObject> defaults;
    
    QJsonObject theme;
    theme["id"] = "dracula-theme";
    theme["name"] = "Dracula Theme";
    theme["version"] = "1.0.0";
    theme["description"] = "Vibrant dark theme for NC IDE styling.";
    theme["publisher"] = "Dracula Org";
    defaults["dracula-theme"] = theme;

    QJsonObject formatter;
    formatter["id"] = "prettier-formatter";
    formatter["name"] = "Prettier Formatter";
    formatter["version"] = "1.5.0";
    formatter["description"] = "Automatic linter-friendly formatter for JS, HTML, and CSS.";
    formatter["publisher"] = "Prettier Org";
    defaults["prettier-formatter"] = formatter;

    QJsonObject linter;
    linter["id"] = "linter-cpp";
    linter["name"] = "C++ Clang-Tidy";
    linter["version"] = "2.1.0";
    linter["description"] = "Static code checker and syntax analysis for C++.";
    linter["publisher"] = "Clang Team";
    defaults["linter-cpp"] = linter;

    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        QString extDir = m_extensionsPath + "/" + it.key();
        QDir().mkpath(extDir);
        
        QFile file(extDir + "/extension.json");
        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly)) {
                QJsonDocument doc(it.value());
                file.write(doc.toJson());
                file.close();
            }
        }
    }
}

void ExtensionManager::scanExtensions() {
    m_extensions.clear();
    
    QDir dir(m_extensionsPath);
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    QSettings settings("NC-IDE", "Extensions");
    QStringList disabledList = settings.value("disabled").toStringList();

    for (const QString& folder : folders) {
        QFile file(m_extensionsPath + "/" + folder + "/extension.json");
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                ExtensionInfo info;
                info.id = obj.value("id").toString();
                info.name = obj.value("name").toString();
                info.version = obj.value("version").toString();
                info.description = obj.value("description").toString();
                info.publisher = obj.value("publisher").toString();
                info.enabled = !disabledList.contains(info.id);
                
                m_extensions.append(info);
            }
        }
    }
    
    emit extensionsChanged();
}

void ExtensionManager::setExtensionEnabled(const QString& id, bool enabled) {
    QSettings settings("NC-IDE", "Extensions");
    QStringList disabledList = settings.value("disabled").toStringList();
    
    if (enabled) {
        disabledList.removeAll(id);
    } else {
        if (!disabledList.contains(id)) {
            disabledList.append(id);
        }
    }
    
    settings.setValue("disabled", disabledList);
    
    for (ExtensionInfo& info : m_extensions) {
        if (info.id == id) {
            info.enabled = enabled;
            break;
        }
    }
    
    emit extensionsChanged();
}

bool ExtensionManager::isExtensionEnabled(const QString& id) const {
    for (const ExtensionInfo& info : m_extensions) {
        if (info.id == id) {
            return info.enabled;
        }
    }
    return true;
}

void ExtensionManager::installMockExtension(const QString& id, const QString& name, const QString& desc, const QString& pub) {
    QString extDir = m_extensionsPath + "/" + id;
    QDir().mkpath(extDir);
    
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["version"] = "1.0.0";
    obj["description"] = desc;
    obj["publisher"] = pub;

    QFile file(extDir + "/extension.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(obj);
        file.write(doc.toJson());
        file.close();
        scanExtensions();
    }
}

void ExtensionManager::registerCommand(const QString& id, const QString& label, std::function<void()> callback) {
    // TODO: Register command with CommandManager
}

void ExtensionManager::registerSidebarView(const QString& id, const QString& title, QWidget* widget) {
    // TODO: Register view in Sidebar
}

void ExtensionManager::showNotification(const QString& message, const QString& type) {
    // TODO: Show UI notification
}

CodeEditor* ExtensionManager::getActiveEditor() {
    // TODO: Return actual active editor
    return nullptr;
}

QString ExtensionManager::getWorkspaceRoot() {
    // TODO: Return actual workspace root
    return QString();
}
