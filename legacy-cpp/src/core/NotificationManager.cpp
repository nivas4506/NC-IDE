// src/core/NotificationManager.cpp
#include "NotificationManager.h"
#include <QUuid>

NotificationManager::NotificationManager(QObject* parent) : QObject(parent) {}

NotificationManager::~NotificationManager() {}

void NotificationManager::showInfo(const QString& title, const QString& message, bool autoDismiss) {
    Notification n{generateId(), title, message, "info", autoDismiss, 0};
    m_notifications.append(n);
    emit notificationAdded(n);
}

void NotificationManager::showWarning(const QString& title, const QString& message, bool autoDismiss) {
    Notification n{generateId(), title, message, "warning", autoDismiss, 0};
    m_notifications.append(n);
    emit notificationAdded(n);
}

void NotificationManager::showError(const QString& title, const QString& message, bool autoDismiss) {
    Notification n{generateId(), title, message, "error", autoDismiss, 0};
    m_notifications.append(n);
    emit notificationAdded(n);
}

QString NotificationManager::showProgress(const QString& title, const QString& message) {
    Notification n{generateId(), title, message, "progress", false, -1};
    m_notifications.append(n);
    emit notificationAdded(n);
    return n.id;
}

void NotificationManager::updateProgress(const QString& id, int progress, const QString& message) {
    for (int i = 0; i < m_notifications.size(); ++i) {
        if (m_notifications[i].id == id) {
            m_notifications[i].progress = progress;
            if (!message.isEmpty()) m_notifications[i].message = message;
            emit notificationUpdated(m_notifications[i]);
            break;
        }
    }
}

void NotificationManager::completeProgress(const QString& id, const QString& message) {
    for (int i = 0; i < m_notifications.size(); ++i) {
        if (m_notifications[i].id == id) {
            if (!message.isEmpty()) {
                m_notifications[i].message = message;
                m_notifications[i].progress = 100;
                m_notifications[i].autoDismiss = true;
                m_notifications[i].type = "info"; // Change to info so it auto-dismisses if desired
                emit notificationUpdated(m_notifications[i]);
            } else {
                emit notificationRemoved(id);
                m_notifications.removeAt(i);
            }
            break;
        }
    }
}

QString NotificationManager::generateId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
