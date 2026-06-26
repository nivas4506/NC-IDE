// src/workspace/WorkspaceManager.h
#pragma once
#include <QObject>
#include <QString>
#include "WorkspaceState.h"

class WorkspaceManager : public QObject {
    Q_OBJECT
public:
    explicit WorkspaceManager(QObject* parent = nullptr);
    ~WorkspaceManager();
    
    void setProjectRoot(const QString& path);
    QString projectRoot() const { return m_projectRoot; }
    
    bool hasWorkspace() const;
    bool loadWorkspace(WorkspaceState& state);
    void saveWorkspace(const WorkspaceState& state);
    
private:
    QString m_projectRoot;
    QString workspaceFilePath() const;
};
