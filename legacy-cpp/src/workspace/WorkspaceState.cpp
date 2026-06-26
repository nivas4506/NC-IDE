// src/workspace/WorkspaceState.cpp
#include "WorkspaceState.h"
#include <QJsonArray>

QJsonObject WorkspaceState::toJson() const {
    QJsonObject obj;
    obj["rootPath"] = rootPath;
    
    QJsonArray tabsArr;
    for (const TabState& ts : openTabs) {
        QJsonObject tabObj;
        tabObj["path"] = ts.path;
        tabObj["cursorLine"] = ts.cursorLine;
        tabObj["cursorCol"] = ts.cursorCol;
        tabObj["scrollPosition"] = ts.scrollPosition;
        tabsArr.append(tabObj);
    }
    obj["openTabs"] = tabsArr;
    obj["activeTab"] = activeTab;
    
    QJsonObject termObj;
    termObj["cwd"] = terminal.cwd;
    QJsonArray histArr;
    for (const QString& h : terminal.history) {
        histArr.append(h);
    }
    termObj["history"] = histArr;
    obj["terminalState"] = termObj;
    
    QJsonObject layObj;
    layObj["explorerWidth"] = layout.explorerWidth;
    layObj["panelHeight"] = layout.panelHeight;
    layObj["activeBottomPanel"] = layout.activeBottomPanel;
    layObj["sourceControlPanelOpen"] = layout.sourceControlPanelOpen;
    obj["layout"] = layObj;
    
    obj["activeRunConfiguration"] = activeRunConfiguration;
    
    return obj;
}

WorkspaceState WorkspaceState::fromJson(const QJsonObject& json) {
    WorkspaceState ws;
    ws.rootPath = json.value("rootPath").toString();
    
    QJsonArray tabsArr = json.value("openTabs").toArray();
    for (int i = 0; i < tabsArr.size(); ++i) {
        QJsonObject tabObj = tabsArr[i].toObject();
        TabState ts;
        ts.path = tabObj.value("path").toString();
        ts.cursorLine = tabObj.value("cursorLine").toInt(1);
        ts.cursorCol = tabObj.value("cursorCol").toInt(1);
        ts.scrollPosition = tabObj.value("scrollPosition").toInt(0);
        ws.openTabs.append(ts);
    }
    ws.activeTab = json.value("activeTab").toString();
    
    QJsonObject termObj = json.value("terminalState").toObject();
    ws.terminal.cwd = termObj.value("cwd").toString();
    QJsonArray histArr = termObj.value("history").toArray();
    for (int i = 0; i < histArr.size(); ++i) {
        ws.terminal.history.append(histArr[i].toString());
    }
    
    QJsonObject layObj = json.value("layout").toObject();
    ws.layout.explorerWidth = layObj.value("explorerWidth").toInt(260);
    ws.layout.panelHeight = layObj.value("panelHeight").toInt(220);
    ws.layout.activeBottomPanel = layObj.value("activeBottomPanel").toString("problems");
    ws.layout.sourceControlPanelOpen = layObj.value("sourceControlPanelOpen").toBool(false);
    
    ws.activeRunConfiguration = json.value("activeRunConfiguration").toString();
    
    return ws;
}
