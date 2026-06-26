// src/ai/AiAssistant.cpp
#include "AiAssistant.h"
#include "../settings/SettingsManager.h"
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

AiAssistant::AiAssistant(NetworkManager* network, QObject* parent) 
    : QObject(parent), m_network(network) {}

AiAssistant::~AiAssistant() {}

void AiAssistant::sendPrompt(const QString& context, const QString& question) {
    QString apiKey = SettingsManager::instance().aiApiKey();
    if (apiKey.isEmpty()) {
        emit errorOccurred("Please set your Gemini API Key in Settings > AI Integration.");
        return;
    }

    QString url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + apiKey;

    QJsonObject textPart;
    textPart["text"] = QString("Context: %1\n\nQuestion: %2").arg(context, question);

    QJsonArray parts;
    parts.append(textPart);

    QJsonObject content;
    content["parts"] = parts;

    QJsonArray contents;
    contents.append(content);

    QJsonObject payload;
    payload["contents"] = contents;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QMap<QByteArray, QByteArray> headers;
    headers["Content-Type"] = "application/json";

    m_network->post(url, data, [this](bool success, const QByteArray& responseData, const QString& errorMsg) {
        if (!success) {
            emit errorOccurred("Network Error: " + errorMsg);
            return;
        }

        QJsonDocument resDoc = QJsonDocument::fromJson(responseData);
        if (!resDoc.isObject()) {
            emit errorOccurred("Invalid JSON response from AI.");
            return;
        }

        QJsonObject resObj = resDoc.object();
        if (resObj.contains("error")) {
            QJsonObject errObj = resObj["error"].toObject();
            emit errorOccurred("API Error: " + errObj["message"].toString());
            return;
        }

        QJsonArray candidates = resObj["candidates"].toArray();
        if (candidates.isEmpty()) {
            emit errorOccurred("No response from AI.");
            return;
        }

        QJsonObject candidate = candidates[0].toObject();
        QJsonObject contentObj = candidate["content"].toObject();
        QJsonArray outParts = contentObj["parts"].toArray();
        if (outParts.isEmpty()) {
            emit errorOccurred("Empty response parts from AI.");
            return;
        }

        QString text = outParts[0].toObject()["text"].toString();
        emit responseReady(text);
    }, headers);
}

void AiAssistant::explainCode(const QString& codeSelection) {
    sendPrompt("The user selected some code.", "Please explain the following code:\n" + codeSelection);
}

void AiAssistant::suggestFix(const QString& errorMessage, const QString& codeSnippet) {
    sendPrompt("The user has a build/runtime error.", 
               "Please suggest a fix for this error:\nError:\n" + errorMessage + "\n\nCode:\n" + codeSnippet);
}

void AiAssistant::simulateDelayAndRespond(const QString& response) {
    QTimer::singleShot(1000, this, [this, response]() {
        emit responseReady(response);
    });
}
