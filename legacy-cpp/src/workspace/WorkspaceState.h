// src/workspace/WorkspaceState.h
#pragma once
#include <QString>
#include <QVector>
#include <QStringList>
#include <QJsonObject>

struct TabState {
    QString path;
    int cursorLine = 1;
    int cursorCol = 1;
    int scrollPosition = 0;
};

struct LayoutState {
    int explorerWidth = 260;
    int panelHeight = 220;
    QString activeBottomPanel = "problems";
    bool sourceControlPanelOpen = false;
};

struct TerminalState {
    QString cwd;
    QStringList history;
};

struct WorkspaceState {
    QString rootPath;
    QVector<TabState> openTabs;
    QString activeTab;
    TerminalState terminal;
    LayoutState layout;
    QString activeRunConfiguration;
    
    QJsonObject toJson() const;
    static WorkspaceState fromJson(const QJsonObject& json);
};
