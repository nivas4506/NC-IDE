// src/session/SessionManager.cpp
#include "SessionManager.h"
#include "../settings/SettingsManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QSet>

SessionManager::SessionManager(TabManager* tabManager, QObject* parent)
    : QObject(parent), m_tabManager(tabManager) {
    connect(&m_autoSaveTimer, &QTimer::timeout, this, &SessionManager::onAutoSaveTimeout);

    SettingsManager& settings = SettingsManager::instance();
    if (settings.autoSaveEnabled()) {
        startAutoSave(settings.autoSaveIntervalSeconds());
    }

    connect(&settings, &SettingsManager::autoSaveSettingsChanged, this, [this](bool enabled, int intervalSeconds) {
        if (enabled) {
            startAutoSave(intervalSeconds);
        } else {
            stopAutoSave();
        }
    });
}

void SessionManager::startAutoSave(int intervalSeconds) {
    m_autoSaveTimer.start(intervalSeconds * 1000);
}

void SessionManager::stopAutoSave() {
    m_autoSaveTimer.stop();
}

QString SessionManager::recoveryDirPath() const {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/recovery";
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path;
}

QString SessionManager::sessionFilePath() const {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path + "/session.json";
}

bool SessionManager::hasRecoverableSession() const {
    QDir dir(recoveryDirPath());
    QStringList filters;
    filters << "*.recovery";
    return !dir.entryList(filters, QDir::Files).isEmpty();
}

void SessionManager::discardRecoverySnapshot() {
    QDir dir(recoveryDirPath());
    QStringList filters;
    filters << "*.recovery";
    for (const QString& filename : dir.entryList(filters, QDir::Files)) {
        dir.remove(filename);
    }
}

void SessionManager::writeRecoverySnapshot() {
    QDir dir(recoveryDirPath());
    QSet<QString> activeHashes;
    QList<EditorTab> openTabs = m_tabManager->allOpenTabs();

    for (int i = 0; i < m_tabManager->tabCount(); ++i) {
        CodeEditor* editor = m_tabManager->editorAt(i);
        if (!editor) continue;
        
        QString filePath = m_tabManager->filePathAt(i);
        bool isDirty = editor->document()->isModified();
        
        if (isDirty) {
            QString key = filePath.isEmpty() ? openTabs[i].displayName : filePath;
            QString hash = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5).toHex();
            activeHashes.insert(hash + ".recovery");
            
            QJsonObject obj;
            obj["filePath"] = filePath;
            obj["displayName"] = openTabs[i].displayName;
            obj["content"] = editor->toPlainText();
            obj["cursorPosition"] = editor->textCursor().position();
            
            QFile file(recoveryDirPath() + "/" + hash + ".recovery");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(QJsonDocument(obj).toJson());
                file.close();
            }
        }
    }
    
    // Cleanup stale recovery files
    QStringList filters;
    filters << "*.recovery";
    for (const QString& filename : dir.entryList(filters, QDir::Files)) {
        if (!activeHashes.contains(filename)) {
            dir.remove(filename);
        }
    }
}

void SessionManager::onAutoSaveTimeout() {
    writeRecoverySnapshot();
}

void SessionManager::saveSession() {
    QJsonArray tabsArray;
    for (int i = 0; i < m_tabManager->tabCount(); ++i) {
        CodeEditor* editor = m_tabManager->editorAt(i);
        if (!editor) continue;
        QJsonObject tabObj;
        tabObj["filePath"] = m_tabManager->filePathAt(i);
        tabObj["cursorPosition"] = editor->textCursor().position();
        tabsArray.append(tabObj);
    }
    
    QJsonObject root;
    root["tabs"] = tabsArray;
    
    CodeEditor* currentEd = m_tabManager->currentEditor();
    int activeIndex = -1;
    if (currentEd) {
        for (int i = 0; i < m_tabManager->tabCount(); ++i) {
            if (m_tabManager->editorAt(i) == currentEd) {
                activeIndex = i;
                break;
            }
        }
    }
    root["activeIndex"] = activeIndex;
    
    QFile file(sessionFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void SessionManager::restoreSession() {
    if (hasRecoverableSession()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr, "Restore Session", 
            "NC IDE detected unsaved work from a previous session that crashed or was closed unexpectedly.\n"
            "Do you want to restore your unsaved changes?",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::Yes) {
            QDir dir(recoveryDirPath());
            QStringList filters;
            filters << "*.recovery";
            
            for (const QString& filename : dir.entryList(filters, QDir::Files)) {
                QFile file(recoveryDirPath() + "/" + filename);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QByteArray data = file.readAll();
                    file.close();
                    
                    QJsonDocument doc = QJsonDocument::fromJson(data);
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        QString filePath = obj["filePath"].toString();
                        QString displayName = obj["displayName"].toString();
                        QString content = obj["content"].toString();
                        int cursorPosition = obj["cursorPosition"].toInt();
                        
                        CodeEditor* editor = nullptr;
                        if (!filePath.isEmpty()) {
                            editor = m_tabManager->openFile(filePath);
                        } else {
                            editor = m_tabManager->newUntitledTab();
                        }
                        
                        if (editor) {
                            editor->setPlainText(content);
                            editor->document()->setModified(true); // Restore as modified
                            
                            QTextCursor cursor = editor->textCursor();
                            cursor.setPosition(qMin(cursorPosition, content.length()));
                            editor->setTextCursor(cursor);
                        }
                    }
                }
            }
            discardRecoverySnapshot();
            return;
        } else {
            discardRecoverySnapshot();
        }
    }

    // Standard session restore
    QFile file(sessionFilePath());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject root = doc.object();
            QJsonArray tabsArray = root["tabs"].toArray();
            
            for (int i = 0; i < tabsArray.size(); ++i) {
                QJsonObject tabObj = tabsArray[i].toObject();
                QString filePath = tabObj["filePath"].toString();
                int cursorPosition = tabObj["cursorPosition"].toInt();
                
                if (!filePath.isEmpty() && QFile::exists(filePath)) {
                    CodeEditor* editor = m_tabManager->openFile(filePath);
                    if (editor) {
                        QTextCursor cursor = editor->textCursor();
                        cursor.setPosition(qMin(cursorPosition, editor->toPlainText().length()));
                        editor->setTextCursor(cursor);
                    }
                }
            }
            
            int activeIndex = root["activeIndex"].toInt(-1);
            if (activeIndex >= 0 && activeIndex < m_tabManager->tabCount()) {
                // Set active tab (openFile already sets active, but this ensures precise index match)
                // QTabWidget can be retrieved via active widget parent
                // But tabManager doesn't expose QTabWidget. Since QTabWidget's current tab gets set on each openFile,
                // we can just let QTabWidget handle the active selection, or retrieve editor and set it.
                CodeEditor* editor = m_tabManager->editorAt(activeIndex);
                if (editor) {
                    // Find index in QTabWidget and select it.
                    // QTabWidget is a private member, but the active editor can be focused.
                    editor->setFocus();
                }
            }
        }
    }
}
