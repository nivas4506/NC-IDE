// src/build/BuildManager.cpp
#include "BuildManager.h"
#include <QDebug>

BuildManager::BuildManager(QObject* parent)
    : QObject(parent) {
}

BuildManager::~BuildManager() {
    cancelBuild();
}

void BuildManager::build(const QString& buildCmd, const QString& workingDir) {
    if (m_isBuilding) return;
    
    m_isBuilding = true;
    emit buildStarted();
    
    m_process = new QProcess(this);
    if (!workingDir.isEmpty()) {
        m_process->setWorkingDirectory(workingDir);
    }
    
    connect(m_process, &QProcess::readyReadStandardOutput, this, &BuildManager::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &BuildManager::onReadyReadStandardError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &BuildManager::onFinished);
    
#ifdef Q_OS_WIN
    QString program = "cmd.exe";
    QStringList arguments;
    arguments << "/c" << buildCmd;
#else
    QString program = "/bin/sh";
    QStringList arguments;
    arguments << "-c" << buildCmd;
#endif

    m_process->start(program, arguments);
}

void BuildManager::cancelBuild() {
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(1000);
        m_process->deleteLater();
        m_process = nullptr;
    }
    m_isBuilding = false;
}

void BuildManager::onReadyReadStandardOutput() {
    if (m_process) {
        QString out = QString::fromLocal8Bit(m_process->readAllStandardOutput());
        emit buildOutput(out, false);
    }
}

void BuildManager::onReadyReadStandardError() {
    if (m_process) {
        QString err = QString::fromLocal8Bit(m_process->readAllStandardError());
        emit buildOutput(err, true);
    }
}

void BuildManager::onFinished(int exitCode, QProcess::ExitStatus status) {
    bool success = (status == QProcess::NormalExit && exitCode == 0);
    emit buildFinished(success, exitCode);
    
    if (m_process) {
        m_process->deleteLater();
        m_process = nullptr;
    }
    m_isBuilding = false;
}
