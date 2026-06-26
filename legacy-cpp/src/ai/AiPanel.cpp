// src/ai/AiPanel.cpp
#include "AiPanel.h"
#include <QVBoxLayout>
#include <QScrollBar>

AiPanel::AiPanel(AiAssistant* assistant, QWidget* parent) 
    : QWidget(parent), m_assistant(assistant) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_chatHistory = new QTextBrowser(this);
    m_chatHistory->setOpenExternalLinks(true);
    m_chatHistory->setStyleSheet(
        "QTextBrowser {"
        "  background-color: #282a36;"
        "  color: #f8f8f2;"
        "  border: none;"
        "  padding: 8px;"
        "}"
    );

    m_inputField = new QLineEdit(this);
    m_inputField->setPlaceholderText("Ask AI...");
    m_inputField->setStyleSheet(
        "QLineEdit {"
        "  background-color: #1e1e24;"
        "  color: #f8f8f2;"
        "  border: 1px solid #44475a;"
        "  padding: 8px;"
        "}"
    );

    layout->addWidget(m_chatHistory);
    layout->addWidget(m_inputField);

    connect(m_inputField, &QLineEdit::returnPressed, this, &AiPanel::onReturnPressed);
    connect(m_assistant, &AiAssistant::responseReady, this, &AiPanel::onResponseReady);
    connect(m_assistant, &AiAssistant::errorOccurred, this, &AiPanel::onError);

    appendMessage("System", "AI Assistant ready. Ask me anything about your code.", "#6272a4");
}

AiPanel::~AiPanel() {}

void AiPanel::onReturnPressed() {
    QString question = m_inputField->text().trimmed();
    if (question.isEmpty()) return;

    appendMessage("You", question, "#bd93f9"); // Accent color
    m_inputField->clear();
    m_inputField->setDisabled(true); // Disable while thinking

    m_assistant->sendPrompt("Project context stub", question);
}

void AiPanel::onResponseReady(const QString& text) {
    appendMessage("NC AI", text, "#50fa7b"); // Success color
    m_inputField->setEnabled(true);
    m_inputField->setFocus();
}

void AiPanel::onError(const QString& error) {
    appendMessage("Error", error, "#ff5555"); // Error color
    m_inputField->setEnabled(true);
}

void AiPanel::appendMessage(const QString& sender, const QString& message, const QString& color) {
    QString html = QString("<b><font color='%1'>%2:</font></b><br>%3<br><br>")
                       .arg(color)
                       .arg(sender)
                       .arg(message);
                       
    // Simple markdown to HTML conversion for newlines
    html.replace("\n", "<br>");
    
    m_chatHistory->append(html);
    m_chatHistory->verticalScrollBar()->setValue(m_chatHistory->verticalScrollBar()->maximum());
}
