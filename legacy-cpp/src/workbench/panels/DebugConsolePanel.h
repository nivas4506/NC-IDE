// src/workbench/panels/DebugConsolePanel.h
#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QLineEdit>

class DebugConsolePanel : public QWidget {
    Q_OBJECT
public:
    explicit DebugConsolePanel(QWidget* parent = nullptr);
    ~DebugConsolePanel();

    void appendOutput(const QString& text, bool isError = false);
    void clear();

signals:
    void expressionEvaluated(const QString& expression);

private slots:
    void onReturnPressed();

private:
    QPlainTextEdit* m_outputView;
    QLineEdit* m_inputField;
};
