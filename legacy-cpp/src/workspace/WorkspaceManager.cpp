// src/workspace/WorkspaceManager.cpp
#include "WorkspaceManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

WorkspaceManager::WorkspaceManager(QObject* parent)
    : QObject(parent) {
}

WorkspaceManager::~WorkspaceManager() {
}

void WorkspaceManager::setProjectRoot(const QString& path) {
    m_projectRoot = path;
}

QString WorkspaceManager::workspaceFilePath() const {
    if (m_projectRoot.isEmpty()) return "";
    return QDir(m_projectRoot).absoluteFilePath(".ncide/workspace.json");
}

bool WorkspaceManager::hasWorkspace() const {
    QString path = workspaceFilePath();
    if (path.isEmpty()) return false;
    return QFile::exists(path);
}

bool WorkspaceManager::loadWorkspace(WorkspaceState& state) {
    QString path = workspaceFilePath();
    if (path.isEmpty() || !QFile::exists(path)) return false;
    
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }
    
    state = WorkspaceState::fromJson(doc.object());
    return true;
}

void WorkspaceManager::saveWorkspace(const WorkspaceState& state) {
    QString path = workspaceFilePath();
    if (path.isEmpty()) return;
    
    // Ensure .ncide directory exists
    QDir projectDir(m_projectRoot);
    if (!projectDir.exists(".ncide")) {
        projectDir.mkdir(".ncide");
    }
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open workspace state file for writing:" << path;
        return;
    }
    
    QJsonDocument doc(state.toJson());
    file.write(doc.toJson());
    file.close();
}
