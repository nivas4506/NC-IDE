// src/session/SessionManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "../tabs/TabManager.h"

class SessionManager : public QObject {
    Q_OBJECT
public:
    explicit SessionManager(TabManager* tabManager, QObject* parent = nullptr);

    // Auto-save: periodically writes dirty buffers to a recovery cache
    void startAutoSave(int intervalSeconds);
    void stopAutoSave();

    // Session persistence: which tabs were open, cursor positions, active tab
    void saveSession();
    void restoreSession();  // call on app startup

    bool hasRecoverableSession() const;
    void discardRecoverySnapshot();

private slots:
    void onAutoSaveTimeout();

private:
    TabManager* m_tabManager;
    QTimer m_autoSaveTimer;
    QString recoveryDirPath() const;
    QString sessionFilePath() const;

    void writeRecoverySnapshot();
};
