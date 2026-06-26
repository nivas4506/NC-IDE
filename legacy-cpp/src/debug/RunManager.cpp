// src/build/RunManager.cpp
#include "RunManager.h"
#include <QProcessEnvironment>
#include <QDebug>

RunManager::RunManager(QObject* parent)
    : QObject(parent) {
}

RunManager::~RunManager() {
    stop();
}

void RunManager::run(const QString& runCmd, const QString& workingDir, const QMap<QString, QString>& env) {
    if (m_isRunning) return;
    
    m_isRunning = true;
    emit runStarted();
    
    m_process = new QProcess(this);
    if (!workingDir.isEmpty()) {
        m_process->setWorkingDirectory(workingDir);
    }
    
    // Set custom environment if provided
    if (!env.isEmpty()) {
        QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
        for (auto it = env.begin(); it != env.end(); ++it) {
            pe.insert(it.key(), it.value());
        }
        m_process->setProcessEnvironment(pe);
    }
    
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RunManager::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &RunManager::onReadyReadStandardError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RunManager::onFinished);
    
#ifdef Q_OS_WIN
    QString program = "cmd.exe";
    QStringList arguments;
    arguments << "/c" << runCmd;
#else
    QString program = "/bin/sh";
    QStringList arguments;
    arguments << "-c" << runCmd;
#endif

    m_process->start(program, arguments);
}

void RunManager::stop() {
    if (m_process && m_isRunning) {
        qint64 pid = m_process->processId();
        if (pid > 0) {
#ifdef Q_OS_WIN
            // Cleanly kill process tree using taskkill
            QProcess::execute("taskkill", QStringList() << "/F" << "/T" << "/PID" << QString::number(pid));
#else
            m_process->terminate();
            if (!m_process->waitForFinished(2000)) {
                m_process->kill();
            }
#endif
        } else {
            m_process->terminate();
            if (!m_process->waitForFinished(2000)) {
                m_process->kill();
            }
        }
        
        m_process->deleteLater();
        m_process = nullptr;
    }
    m_isRunning = false;
}

void RunManager::onReadyReadStandardOutput() {
    if (m_process) {
        QString out = QString::fromLocal8Bit(m_process->readAllStandardOutput());
        emit runOutput(out, false);
    }
}

void RunManager::onReadyReadStandardError() {
    if (m_process) {
        QString err = QString::fromLocal8Bit(m_process->readAllStandardError());
        emit runOutput(err, true);
    }
}

void RunManager::onFinished(int exitCode, QProcess::ExitStatus /* status */) {
    emit runFinished(exitCode);
    
    if (m_process) {
        m_process->deleteLater();
        m_process = nullptr;
    }
    m_isRunning = false;
}
