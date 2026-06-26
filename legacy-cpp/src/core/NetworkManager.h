// src/core/NetworkManager.h
#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QString>
#include <functional>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    using ResponseCallback = std::function<void(bool success, const QByteArray& data, const QString& errorMsg)>;

    void get(const QString& url, ResponseCallback callback);
    void post(const QString& url, const QByteArray& data, ResponseCallback callback, const QMap<QByteArray, QByteArray>& headers = QMap<QByteArray, QByteArray>());

private:
    QNetworkAccessManager* m_manager;
};
