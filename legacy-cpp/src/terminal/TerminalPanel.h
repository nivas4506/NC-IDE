// src/terminal/TerminalPanel.h
#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QProcess>

class TerminalPanel : public QWidget {
    Q_OBJECT
public:
    explicit TerminalPanel(QWidget* parent = nullptr);
    ~TerminalPanel();
    
    void setWorkingDirectory(const QString& dir);
    QString workingDirectory() const { return m_workingDir; }
    
    QStringList history() const { return m_history; }
    void setHistory(const QStringList& hist);
    
    void runCommand(const QString& cmdText);

private slots:
    void onReturnPressed();
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    QPlainTextEdit* m_outputView;
    QLineEdit* m_commandInput;
    QProcess* m_process = nullptr;
    QString m_workingDir;
    QStringList m_history;
    int m_historyIndex = -1;
    
    void startShell();
    void stopShell();
    
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};
