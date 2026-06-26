// src/ai/AiPanel.h
#pragma once
#include <QWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include "AiAssistant.h"

class AiPanel : public QWidget {
    Q_OBJECT
public:
    explicit AiPanel(AiAssistant* assistant, QWidget* parent = nullptr);
    ~AiPanel();

private slots:
    void onReturnPressed();
    void onResponseReady(const QString& text);
    void onError(const QString& error);

private:
    AiAssistant* m_assistant;
    QTextBrowser* m_chatHistory;
    QLineEdit* m_inputField;

    void appendMessage(const QString& sender, const QString& message, const QString& color);
};
