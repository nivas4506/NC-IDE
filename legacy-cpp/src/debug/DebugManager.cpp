// src/debug/DebugManager.cpp
#include "DebugManager.h"
#include <QDebug>
#include <QFileInfo>

DebugManager::DebugManager(QObject* parent) 
    : QObject(parent), m_process(new QProcess(this)), m_isDebugging(false) {
    
    connect(m_process, &QProcess::readyReadStandardOutput, this, &DebugManager::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &DebugManager::onReadyReadStandardError);
    connect(m_process, &QProcess::errorOccurred, this, &DebugManager::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), 
            this, &DebugManager::onProcessFinished);
}

DebugManager::~DebugManager() {
    stopDebugSession();
}

bool DebugManager::startDebugSession(const QString& executablePath, const QString& workingDir) {
    if (m_isDebugging) stopDebugSession();

    QFileInfo info(executablePath);
    if (!info.exists()) {
        emit outputReceived("Error: Executable not found at " + executablePath + "\n", true);
        return false;
    }

    m_process->setWorkingDirectory(workingDir);
    m_process->start("gdb", QStringList() << "--interpreter=mi2" << "-q" << executablePath);
    
    if (!m_process->waitForStarted(3000)) {
        emit outputReceived("Error: Failed to start GDB.\n", true);
        return false;
    }

    m_isDebugging = true;
    emit sessionStarted();
    
    // Apply existing breakpoints
    for (const auto& bp : m_breakpoints) {
        if (bp.enabled) {
            sendCommand(QString("-break-insert %1:%2").arg(QFileInfo(bp.filePath).fileName()).arg(bp.line));
        }
    }

    sendCommand("-exec-run");
    return true;
}

void DebugManager::stopDebugSession() {
    if (m_isDebugging && m_process->state() == QProcess::Running) {
        sendCommand("-gdb-exit");
        if (!m_process->waitForFinished(2000)) {
            m_process->kill();
        }
    }
}

void DebugManager::toggleBreakpoint(const QString& file, int line) {
    for (int i = 0; i < m_breakpoints.size(); ++i) {
        if (m_breakpoints[i].filePath == file && m_breakpoints[i].line == line) {
            m_breakpoints.removeAt(i);
            if (m_isDebugging) {
                // To properly remove, we'd need the GDB breakpoint number, but for stub:
                // sending a clear command as CLI through MI:
                sendCommand(QString("-interpreter-exec console \"clear %1:%2\"").arg(QFileInfo(file).fileName()).arg(line));
            }
            return;
        }
    }
    
    m_breakpoints.append({file, line, true});
    if (m_isDebugging) {
        sendCommand(QString("-break-insert %1:%2").arg(QFileInfo(file).fileName()).arg(line));
    }
}

void DebugManager::continueExecution() { sendCommand("-exec-continue"); }
void DebugManager::stepOver() { sendCommand("-exec-next"); }
void DebugManager::stepInto() { sendCommand("-exec-step"); }
void DebugManager::stepOut() { sendCommand("-exec-finish"); }

void DebugManager::evaluateExpression(const QString& expr) {
    if (m_isDebugging) {
        sendCommand("-data-evaluate-expression " + expr);
    } else {
        emit evaluationResult("Not debugging.\n");
    }
}

void DebugManager::sendCommand(const QString& cmd) {
    if (m_process->state() == QProcess::Running) {
        m_process->write((cmd + "\n").toUtf8());
    }
}

void DebugManager::onReadyReadStandardOutput() {
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    emit outputReceived(output, false);
    parseGdbOutput(output);
}

void DebugManager::onReadyReadStandardError() {
    QString output = QString::fromUtf8(m_process->readAllStandardError());
    emit outputReceived(output, true);
}

void DebugManager::parseGdbOutput(const QString& output) {
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        // Handle stream output
        if (trimmed.startsWith("~\"")) {
            // Unescape C-string
            QString text = trimmed.mid(2, trimmed.length() - 3);
            text.replace("\\n", "\n").replace("\\t", "\t").replace("\\\"", "\"");
            // Do not emit here, to avoid duplication since onReadyReadStandardOutput already emits the raw output.
            // But we could format it if we wanted.
        }
        
        // Handle Async Exec records
        if (trimmed.startsWith("*stopped")) {
            if (trimmed.contains("reason=\"breakpoint-hit\"")) {
                emit breakpointHit("unknown", 0); // Need to parse frame info for real line
            }
        }
        
        // Handle Result records for evaluation
        if (trimmed.startsWith("^done,value=")) {
            int startIdx = trimmed.indexOf("value=\"") + 7;
            int endIdx = trimmed.lastIndexOf("\"");
            if (startIdx >= 7 && endIdx > startIdx) {
                emit evaluationResult(trimmed.mid(startIdx, endIdx - startIdx));
            }
        }
    }
}

void DebugManager::onProcessError(QProcess::ProcessError error) {
    emit outputReceived(QString("GDB Process Error: %1\n").arg(error), true);
}

void DebugManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    m_isDebugging = false;
    emit sessionEnded();
    emit outputReceived(QString("Debug session ended (Code %1)\n").arg(exitCode), false);
}
