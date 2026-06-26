// src/build/RunManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QProcess>
#include <QMap>

class RunManager : public QObject {
    Q_OBJECT
public:
    explicit RunManager(QObject* parent = nullptr);
    ~RunManager();
    
    bool isRunning() const { return m_isRunning; }
    void run(const QString& runCmd, const QString& workingDir, const QMap<QString, QString>& env = {});
    void stop();
    
signals:
    void runStarted();
    void runOutput(const QString& text, bool isError);
    void runFinished(int exitCode);
    
private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onFinished(int exitCode, QProcess::ExitStatus status);

private:
    QProcess* m_process = nullptr;
    bool m_isRunning = false;
};
