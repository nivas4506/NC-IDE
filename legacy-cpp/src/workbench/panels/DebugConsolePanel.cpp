// src/workbench/panels/DebugConsolePanel.cpp
#include "DebugConsolePanel.h"
#include <QVBoxLayout>
#include <QScrollBar>

DebugConsolePanel::DebugConsolePanel(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_outputView = new QPlainTextEdit(this);
    m_outputView->setReadOnly(true);
    m_outputView->setFrameShape(QFrame::NoFrame);
    
    // Add placeholder text
    m_outputView->appendPlainText("Start a debug session to evaluate expressions.\n");

    m_inputField = new QLineEdit(this);
    m_inputField->setPlaceholderText("> Evaluate expression...");
    m_inputField->setFrame(false);

    layout->addWidget(m_outputView);
    layout->addWidget(m_inputField);

    connect(m_inputField, &QLineEdit::returnPressed, this, &DebugConsolePanel::onReturnPressed);
}

DebugConsolePanel::~DebugConsolePanel() {}

void DebugConsolePanel::appendOutput(const QString& text, bool isError) {
    QTextCharFormat format;
    if (isError) {
        format.setForeground(QColor("#ff5555")); // Dracula error color
    } else {
        format.setForeground(QColor("#f8f8f2")); // Dracula text color
    }

    QTextCursor cursor(m_outputView->document());
    cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(format);
    cursor.insertText(text);
    
    if (!text.endsWith('\n')) {
        cursor.insertText("\n");
    }

    m_outputView->verticalScrollBar()->setValue(m_outputView->verticalScrollBar()->maximum());
}

void DebugConsolePanel::clear() {
    m_outputView->clear();
}

void DebugConsolePanel::onReturnPressed() {
    QString expr = m_inputField->text().trimmed();
    if (!expr.isEmpty()) {
        appendOutput("> " + expr);
        emit expressionEvaluated(expr);
        m_inputField->clear();
    }
}
