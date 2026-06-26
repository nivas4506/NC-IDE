// src/git/GitManager.cpp
#include "GitManager.h"
#include <QProcess>
#include <QDebug>

GitManager::GitManager(QObject* parent)
    : QObject(parent) {
}

void GitManager::setRootPath(const QString& path) {
    m_rootPath = path;
    refreshStatus();
}

void GitManager::runGitCommand(const QStringList& args, const QString& opName, 
                               std::function<void(const QString& stdoutText, const QString& stderrText, int exitCode)> callback) {
    if (m_rootPath.isEmpty() && opName != "clone") {
        emit operationFailed(opName, "No project path set");
        return;
    }
    
    QProcess* process = new QProcess(this);
    if (!m_rootPath.isEmpty()) {
        process->setWorkingDirectory(m_rootPath);
    }
    
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process, callback, opName](int exitCode, QProcess::ExitStatus status) {
        QString stdoutText = QString::fromLocal8Bit(process->readAllStandardOutput());
        QString stderrText = QString::fromLocal8Bit(process->readAllStandardError());
        
        if (status == QProcess::CrashExit) {
            emit operationFailed(opName, "Process crashed: " + stderrText);
        } else {
            callback(stdoutText, stderrText, exitCode);
        }
        process->deleteLater();
    });
    
    connect(process, &QProcess::errorOccurred, this, [this, process, opName](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            emit operationFailed(opName, "Git executable not found in PATH.");
        }
        process->deleteLater();
    });
    
    process->start("git", args);
}

void GitManager::refreshStatus() {
    runGitCommand({"status", "--porcelain=v2", "-b"}, "status", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode != 0) {
            emit isGitRepo(false);
            return;
        }
        emit isGitRepo(true);
        
        QVector<GitFileStatus> statuses;
        QString currentBranch = "DETACHED";
        
        QStringList lines = stdoutText.split('\n');
        for (const QString& line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) continue;
            
            if (trimmed.startsWith("# branch.head ")) {
                currentBranch = trimmed.mid(14).trimmed();
                continue;
            }
            if (trimmed.startsWith("#")) continue;
            
            QStringList tokens = trimmed.split(' ', Qt::SkipEmptyParts);
            if (tokens.isEmpty()) continue;
            
            QString type = tokens[0];
            if (type == "?") {
                if (tokens.size() >= 2) {
                    GitFileStatus fs;
                    fs.path = tokens.mid(1).join(' ');
                    fs.state = "Untracked";
                    fs.staged = false;
                    statuses.append(fs);
                }
            } else if (type == "1" || type == "2" || type == "u") {
                if (tokens.size() < 9) continue;
                QString xy = tokens[1];
                if (xy.length() < 2) continue;
                QChar x = xy[0];
                QChar y = xy[1];
                
                QString path;
                if (type == "2") {
                    QString rest = tokens.mid(8).join(' ');
                    QStringList paths = rest.split('\t');
                    path = paths.size() >= 2 ? paths[1] : rest;
                } else {
                    path = tokens.mid(8).join(' ');
                }
                
                QString state = "Modified";
                if (type == "u") {
                    state = "Conflicted";
                } else if (x == 'A' || y == 'A') {
                    state = "Added";
                } else if (x == 'D' || y == 'D') {
                    state = "Deleted";
                } else if (x == 'R' || y == 'R') {
                    state = "Renamed";
                }
                
                if (x != '.') {
                    GitFileStatus fs;
                    fs.path = path;
                    fs.state = state;
                    fs.staged = true;
                    statuses.append(fs);
                }
                if (y != '.') {
                    GitFileStatus fs;
                    fs.path = path;
                    fs.state = state;
                    fs.staged = false;
                    statuses.append(fs);
                }
            }
        }
        
        emit statusChanged(statuses);
        refreshBranches();
        refreshLog();
    });
}

void GitManager::refreshBranches() {
    runGitCommand({"branch", "--no-color"}, "branches", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode != 0) return;
        
        QStringList branches;
        QString currentBranch;
        
        QStringList lines = stdoutText.split('\n');
        for (const QString& line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) continue;
            
            if (trimmed.startsWith('*')) {
                currentBranch = trimmed.mid(1).trimmed();
                branches.append(currentBranch);
            } else {
                branches.append(trimmed);
            }
        }
        emit branchesChanged(branches, currentBranch);
    });
}

void GitManager::refreshLog() {
    runGitCommand({"log", "--pretty=format:%H|%an|%ad|%s", "--date=short", "-n", "30"}, "log", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode != 0) return;
        
        QVector<GitCommit> commits;
        QStringList lines = stdoutText.split('\n');
        for (const QString& line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) continue;
            
            QStringList parts = trimmed.split('|');
            if (parts.size() >= 4) {
                GitCommit c;
                c.hash = parts[0];
                c.author = parts[1];
                c.date = parts[2];
                c.message = parts.mid(3).join('|');
                commits.append(c);
            }
        }
        emit logChanged(commits);
    });
}

void GitManager::initRepository() {
    runGitCommand({"init"}, "init", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode == 0) {
            refreshStatus();
        } else {
            emit operationFailed("init", stderrText);
        }
    });
}

void GitManager::cloneRepository(const QString& url, const QString& destPath) {
    runGitCommand({"clone", url, destPath}, "clone", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode == 0) {
            refreshStatus();
        } else {
            emit operationFailed("clone", stderrText);
        }
    });
}

void GitManager::stageFile(const QString& relativePath) {
    runGitCommand({"add", "--", relativePath}, "stage", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode == 0) {
            refreshStatus();
        } else {
            emit operationFailed("stage", stderrText);
        }
    });
}

void GitManager::unstageFile(const QString& relativePath) {
    runGitCommand({"restore", "--staged", "--", relativePath}, "unstage", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode == 0) {
            refreshStatus();
        } else {
            emit operationFailed("unstage", stderrText);
        }
    });
}

void GitManager::commitChanges(const QString& message) {
    // Check if anything is staged first. We do status check.
    runGitCommand({"diff", "--cached", "--name-only"}, "precommit", [this, message](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (stdoutText.trimmed().isEmpty()) {
            // Nothing staged, stage all
            runGitCommand({"add", "-A"}, "stageall", [this, message](const QString& stdoutText, const QString& stderrText, int exitCode) {
                if (exitCode == 0) {
                    commitChanges(message); // recurse to commit
                } else {
                    emit commitCompleted(false, stderrText);
                }
            });
        } else {
            // Commit staged
            runGitCommand({"commit", "-m", message}, "commit", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
                if (exitCode == 0) {
                    emit commitCompleted(true, "");
                    refreshStatus();
                } else {
                    emit commitCompleted(false, stderrText);
                }
            });
        }
    });
}

void GitManager::pull() {
    runGitCommand({"pull", "--ff-only"}, "pull", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        emit pullCompleted(exitCode == 0, stdoutText + "\n" + stderrText);
        refreshStatus();
    });
}

void GitManager::push() {
    runGitCommand({"push"}, "push", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        emit pushCompleted(exitCode == 0, stdoutText + "\n" + stderrText);
        refreshStatus();
    });
}

void GitManager::checkoutBranch(const QString& name) {
    runGitCommand({"checkout", name}, "checkout", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode == 0) {
            refreshStatus();
        } else {
            emit operationFailed("checkout", stderrText);
        }
    });
}

void GitManager::createBranch(const QString& name) {
    runGitCommand({"checkout", "-b", name}, "createbranch", [this](const QString& stdoutText, const QString& stderrText, int exitCode) {
        if (exitCode == 0) {
            refreshStatus();
        } else {
            emit operationFailed("createbranch", stderrText);
        }
    });
}
