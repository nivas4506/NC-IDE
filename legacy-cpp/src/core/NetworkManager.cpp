// src/core/NetworkManager.cpp
#include "NetworkManager.h"
#include <QNetworkRequest>
#include <QUrl>

NetworkManager::NetworkManager(QObject* parent) : QObject(parent), m_manager(new QNetworkAccessManager(this)) {}

NetworkManager::~NetworkManager() {}

void NetworkManager::get(const QString& url, ResponseCallback callback) {
    QNetworkRequest request((QUrl(url)));
    QNetworkReply* reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        if (reply->error() == QNetworkReply::NoError) {
            callback(true, reply->readAll(), QString());
        } else {
            callback(false, QByteArray(), reply->errorString());
        }
        reply->deleteLater();
    });
}

void NetworkManager::post(const QString& url, const QByteArray& data, ResponseCallback callback, const QMap<QByteArray, QByteArray>& headers) {
    QNetworkRequest request((QUrl(url)));
    
    bool hasContentType = false;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key(), it.value());
        if (it.key().toLower() == "content-type") hasContentType = true;
    }
    if (!hasContentType) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    QNetworkReply* reply = m_manager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        if (reply->error() == QNetworkReply::NoError) {
            callback(true, reply->readAll(), QString());
        } else {
            callback(false, QByteArray(), reply->errorString());
        }
        reply->deleteLater();
    });
}
