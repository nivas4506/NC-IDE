// src/terminal/TerminalPanel.cpp
#include "TerminalPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QScrollBar>

TerminalPanel::TerminalPanel(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Output text area
    m_outputView = new QPlainTextEdit(this);
    m_outputView->setReadOnly(true);
    m_outputView->setStyleSheet(
        "background-color: #121212;"
        "color: #3FB950;" // Green text matching --success
        "font-family: 'JetBrains Mono', 'Consolas', 'Courier New', monospace;"
        "font-size: 10pt;"
        "border: none;"
    );
    mainLayout->addWidget(m_outputView, 1);
    
    // Input row widget
    QWidget* inputRow = new QWidget(this);
    inputRow->setStyleSheet("background-color: #1a1a1a; border-top: 1px solid #2a2a2a;");
    QHBoxLayout* inputLayout = new QHBoxLayout(inputRow);
    inputLayout->setContentsMargins(6, 4, 6, 4);
    inputLayout->setSpacing(4);
    
    QLabel* promptLabel = new QLabel("> ", inputRow);
    promptLabel->setStyleSheet("color: #3FB950; font-family: monospace; font-weight: bold; font-size: 10pt;");
    inputLayout->addWidget(promptLabel);
    
    m_commandInput = new QLineEdit(inputRow);
    m_commandInput->setStyleSheet(
        "background-color: transparent;"
        "color: #E0E0E0;"
        "border: none;"
        "font-family: 'JetBrains Mono', 'Consolas', 'Courier New', monospace;"
        "font-size: 10pt;"
    );
    inputLayout->addWidget(m_commandInput, 1);
    
    mainLayout->addWidget(inputRow);
    
    // Install event filter for command history arrow navigation
    m_commandInput->installEventFilter(this);
    
    connect(m_commandInput, &QLineEdit::returnPressed, this, &TerminalPanel::onReturnPressed);
    
    // Start shell in default directory
    startShell();
}

TerminalPanel::~TerminalPanel() {
    stopShell();
}

void TerminalPanel::setWorkingDirectory(const QString& dir) {
    if (m_workingDir != dir) {
        m_workingDir = dir;
        startShell();
    }
}

void TerminalPanel::setHistory(const QStringList& hist) {
    m_history = hist;
}

void TerminalPanel::runCommand(const QString& cmdText) {
    if (m_process && m_process->state() == QProcess::Running) {
        m_outputView->appendPlainText("> " + cmdText);
        m_process->write((cmdText + "\n").toLocal8Bit());
        m_history.append(cmdText);
        m_historyIndex = -1;
    }
}

void TerminalPanel::onReturnPressed() {
    QString cmd = m_commandInput->text().trimmed();
    if (cmd.isEmpty()) return;
    
    runCommand(cmd);
    m_commandInput->clear();
}

void TerminalPanel::onReadyReadStandardOutput() {
    if (m_process) {
        QByteArray data = m_process->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(data);
        m_outputView->appendPlainText(text);
        
        // Auto scroll to bottom
        m_outputView->verticalScrollBar()->setValue(m_outputView->verticalScrollBar()->maximum());
    }
}

void TerminalPanel::onReadyReadStandardError() {
    if (m_process) {
        QByteArray data = m_process->readAllStandardError();
        QString text = QString::fromLocal8Bit(data);
        m_outputView->appendPlainText(text);
        
        // Auto scroll to bottom
        m_outputView->verticalScrollBar()->setValue(m_outputView->verticalScrollBar()->maximum());
    }
}

void TerminalPanel::startShell() {
    stopShell();
    
    m_process = new QProcess(this);
    if (!m_workingDir.isEmpty()) {
        m_process->setWorkingDirectory(m_workingDir);
    }
    
    connect(m_process, &QProcess::readyReadStandardOutput, this, &TerminalPanel::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &TerminalPanel::onReadyReadStandardError);
    
#ifdef Q_OS_WIN
    m_process->start("powershell.exe", QStringList() << "-NoLogo" << "-NoExit");
#else
    m_process->start("/bin/bash", QStringList() << "-i");
#endif

    // Initial feed to trigger prompt
    m_process->write("\n");
}

void TerminalPanel::stopShell() {
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(1000);
        m_process->deleteLater();
        m_process = nullptr;
    }
}

bool TerminalPanel::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_commandInput && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Up) {
            if (!m_history.isEmpty()) {
                if (m_historyIndex == -1) {
                    m_historyIndex = m_history.size() - 1;
                } else if (m_historyIndex > 0) {
                    m_historyIndex--;
                }
                m_commandInput->setText(m_history[m_historyIndex]);
            }
            return true;
        } else if (keyEvent->key() == Qt::Key_Down) {
            if (!m_history.isEmpty()) {
                if (m_historyIndex != -1) {
                    if (m_historyIndex < m_history.size() - 1) {
                        m_historyIndex++;
                        m_commandInput->setText(m_history[m_historyIndex]);
                    } else {
                        m_historyIndex = -1;
                        m_commandInput->clear();
                    }
                }
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
