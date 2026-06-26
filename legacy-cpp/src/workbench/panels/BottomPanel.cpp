// src/workbench/panels/BottomPanel.cpp
#include "BottomPanel.h"
#include <QVBoxLayout>

BottomPanel::BottomPanel(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::South);
    layout->addWidget(m_tabWidget);

    m_problemsPanel = new ProblemsPanel(m_tabWidget);
    m_tabWidget->addTab(m_problemsPanel, "Problems");

    m_outputView = new QPlainTextEdit(m_tabWidget);
    m_outputView->setReadOnly(true);
    m_outputView->setStyleSheet(
        "background-color: #121212; color: #e0e0e0; font-family: 'JetBrains Mono', 'Consolas', 'Courier New', monospace; font-size: 10pt; border: none;"
    );
    m_tabWidget->addTab(m_outputView, "Output");

    m_debugConsole = new DebugConsolePanel(this);
    m_tabWidget->addTab(m_debugConsole, "Debug Console");

    m_terminalPanel = new TerminalPanel(m_tabWidget);
    m_tabWidget->addTab(m_terminalPanel, "Terminal");


    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; background-color: #191a21; }"
        "QTabBar::tab { background-color: #191a21; color: #6272a4; padding: 6px 12px; border: 1px solid #282a36; border-top: none; }"
        "QTabBar::tab:selected { background-color: #21222c; color: #f8f8f2; font-weight: bold; border-bottom: 2px solid #50fa7b; }"
        "QTabBar::tab:hover { color: #f8f8f2; background-color: #21222c; }"
    );
}

void BottomPanel::showTerminal() {
    m_tabWidget->setCurrentWidget(m_terminalPanel);
}

void BottomPanel::showProblems() {
    m_tabWidget->setCurrentWidget(m_problemsPanel);
}

void BottomPanel::showOutput() {
    m_tabWidget->setCurrentWidget(m_outputView);
}

void BottomPanel::showDebugConsole() {
    m_tabWidget->setCurrentIndex(3);
}

