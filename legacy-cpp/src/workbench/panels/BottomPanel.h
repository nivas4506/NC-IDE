// src/workbench/panels/BottomPanel.h
#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include "ProblemsPanel.h"
#include "../../terminal/TerminalPanel.h"
#include "DebugConsolePanel.h"

class BottomPanel : public QWidget {
    Q_OBJECT
public:
    explicit BottomPanel(QWidget* parent = nullptr);

    ProblemsPanel* problemsPanel() const { return m_problemsPanel; }
    QPlainTextEdit* outputView() const { return m_outputView; }
    TerminalPanel* terminalPanel() const { return m_terminalPanel; }
    DebugConsolePanel* debugConsole() const { return m_debugConsole; }

    void showTerminal();
    void showProblems();
    void showOutput();
    void showDebugConsole();



private:
    QTabWidget* m_tabWidget;
    ProblemsPanel* m_problemsPanel;
    QPlainTextEdit* m_outputView;
    TerminalPanel* m_terminalPanel;
    
    DebugConsolePanel* m_debugConsole;
};
