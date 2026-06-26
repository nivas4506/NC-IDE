// src/git/GitManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include <functional>

struct GitFileStatus {
    QString path;
    QString state; // "Added", "Modified", "Deleted", "Renamed", "Untracked", "Conflicted"
    bool staged;
};

struct GitCommit {
    QString hash;
    QString author;
    QString date;
    QString message;
};

class GitManager : public QObject {
    Q_OBJECT
public:
    explicit GitManager(QObject* parent = nullptr);
    
    void setRootPath(const QString& path);
    QString rootPath() const { return m_rootPath; }
    
    void refreshStatus();
    void refreshLog();
    void refreshBranches();
    
    void initRepository();
    void cloneRepository(const QString& url, const QString& destPath);
    void stageFile(const QString& relativePath);
    void unstageFile(const QString& relativePath);
    void commitChanges(const QString& message);
    
    void pull();
    void push();
    
    void checkoutBranch(const QString& name);
    void createBranch(const QString& name);
    
signals:
    void statusChanged(const QVector<GitFileStatus>& status);
    void logChanged(const QVector<GitCommit>& log);
    void branchesChanged(const QStringList& branches, const QString& currentBranch);
    void commitCompleted(bool success, const QString& error);
    void pullCompleted(bool success, const QString& output);
    void pushCompleted(bool success, const QString& output);
    void operationFailed(const QString& op, const QString& stderrText);
    void isGitRepo(bool status);

private:
    QString m_rootPath;
    
    void runGitCommand(const QStringList& args, const QString& opName, 
                       std::function<void(const QString& stdoutText, const QString& stderrText, int exitCode)> callback);
};
