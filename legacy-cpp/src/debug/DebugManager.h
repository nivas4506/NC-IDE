// src/debug/DebugManager.h
#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
#include <QVector>

struct Breakpoint {
    QString filePath;
    int line;
    bool enabled = true;
};

class DebugManager : public QObject {
    Q_OBJECT
public:
    explicit DebugManager(QObject* parent = nullptr);
    ~DebugManager();

    bool startDebugSession(const QString& executablePath, const QString& workingDir);
    void stopDebugSession();
    
    // Breakpoints
    void toggleBreakpoint(const QString& file, int line);
    QVector<Breakpoint> breakpoints() const { return m_breakpoints; }

    // Execution control
    void continueExecution();
    void stepOver();
    void stepInto();
    void stepOut();

    // Evaluation
    void evaluateExpression(const QString& expr);

signals:
    void sessionStarted();
    void sessionEnded();
    void breakpointHit(const QString& file, int line);
    void outputReceived(const QString& text, bool isError);
    void evaluationResult(const QString& result);

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess* m_process;
    QVector<Breakpoint> m_breakpoints;
    bool m_isDebugging;

    void sendCommand(const QString& cmd);
    void parseGdbOutput(const QString& output);
};
