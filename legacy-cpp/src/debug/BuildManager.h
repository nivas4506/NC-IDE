// src/build/BuildManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QProcess>

class BuildManager : public QObject {
    Q_OBJECT
public:
    explicit BuildManager(QObject* parent = nullptr);
    ~BuildManager();
    
    bool isBuilding() const { return m_isBuilding; }
    void build(const QString& buildCmd, const QString& workingDir);
    void cancelBuild();
    
signals:
    void buildStarted();
    void buildOutput(const QString& text, bool isError);
    void buildFinished(bool success, int exitCode);
    
private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onFinished(int exitCode, QProcess::ExitStatus status);

private:
    QProcess* m_process = nullptr;
    bool m_isBuilding = false;
};
