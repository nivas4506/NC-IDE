// src/core/NotificationManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QVector>

struct Notification {
    QString id;
    QString title;
    QString message;
    QString type; // "info", "warning", "error", "progress"
    bool autoDismiss;
    int progress; // 0-100, -1 for indeterminate
};

class NotificationManager : public QObject {
    Q_OBJECT
public:
    explicit NotificationManager(QObject* parent = nullptr);
    ~NotificationManager();

    void showInfo(const QString& title, const QString& message, bool autoDismiss = true);
    void showWarning(const QString& title, const QString& message, bool autoDismiss = true);
    void showError(const QString& title, const QString& message, bool autoDismiss = false);
    
    // Progress notification returns an ID that can be used to update it
    QString showProgress(const QString& title, const QString& message);
    void updateProgress(const QString& id, int progress, const QString& message = QString());
    void completeProgress(const QString& id, const QString& message = QString());

    QVector<Notification> activeNotifications() const { return m_notifications; }

signals:
    void notificationAdded(const Notification& notification);
    void notificationUpdated(const Notification& notification);
    void notificationRemoved(const QString& id);

private:
    QVector<Notification> m_notifications;
    QString generateId();
};
