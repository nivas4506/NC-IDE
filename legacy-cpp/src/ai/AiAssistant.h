// src/ai/AiAssistant.h
#pragma once
#include <QObject>
#include <QString>
#include "../core/NetworkManager.h"

class AiAssistant : public QObject {
    Q_OBJECT
public:
    explicit AiAssistant(NetworkManager* network, QObject* parent = nullptr);
    ~AiAssistant();

    void sendPrompt(const QString& context, const QString& question);
    void explainCode(const QString& codeSelection);
    void suggestFix(const QString& errorMessage, const QString& codeSnippet);

signals:
    void responseReady(const QString& text);
    void errorOccurred(const QString& errorMsg);

private:
    NetworkManager* m_network;
    
    // Stub for actual API integration
    void simulateDelayAndRespond(const QString& response);
};
