// src/search/ProjectSearchEngine.cpp
#include "ProjectSearchEngine.h"
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>

ProjectSearchEngine::ProjectSearchEngine(QObject* parent)
    : QObject(parent) {
    m_fileFilters = {"*.c", "*.cpp", "*.h", "*.hpp", "*.py", "*.java", "*.js", "*.html", "*.css"};
}

void ProjectSearchEngine::setProjectRoot(const QString& rootPath) {
    m_projectRoot = QDir::cleanPath(rootPath);
}

void ProjectSearchEngine::setFileFilters(const QStringList& extensions) {
    m_fileFilters = extensions;
}

QStringList ProjectSearchEngine::collectProjectFiles() const {
    QStringList files;
    if (m_projectRoot.isEmpty()) return files;

    QDirIterator it(m_projectRoot, m_fileFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QString cleanPath = QDir::cleanPath(path);
        
        // Skip common build or vcs folders
        QStringList parts = cleanPath.split('/');
        bool skip = false;
        for (const QString& part : parts) {
            if (part == ".git" || part == "node_modules" || part == "build" || part == ".vscode" || part == "out" || part == ".gemini") {
                skip = true;
                break;
            }
        }
        if (!skip) {
            files.append(cleanPath);
        }
    }
    return files;
}

QList<ProjectSearchResult> ProjectSearchEngine::searchFile(const QString& path, const QRegularExpression& pattern) const {
    QList<ProjectSearchResult> results;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return results;
    }

    QTextStream in(&file);
    int lineNum = 1;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatchIterator matchIt = pattern.globalMatch(line);
        while (matchIt.hasNext()) {
            QRegularExpressionMatch match = matchIt.next();
            ProjectSearchResult res;
            res.filePath = path;
            res.lineNumber = lineNum;
            res.columnNumber = match.capturedStart() + 1;
            res.lineText = line;
            results.append(res);
        }
        lineNum++;
    }
    file.close();
    return results;
}

QList<ProjectSearchResult> ProjectSearchEngine::search(const QString& term, bool matchCase, bool wholeWord) {
    QList<ProjectSearchResult> results;
    if (term.isEmpty()) return results;

    QString patternStr = QRegularExpression::escape(term);
    if (wholeWord) {
        patternStr = "\\b" + patternStr + "\\b";
    }

    QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
    if (!matchCase) {
        options |= QRegularExpression::CaseInsensitiveOption;
    }

    QRegularExpression pattern(patternStr, options);
    QStringList files = collectProjectFiles();
    int totalFiles = files.size();

    for (int i = 0; i < totalFiles; ++i) {
        results.append(searchFile(files[i], pattern));
        emit searchProgress(i + 1, totalFiles);
        QCoreApplication::processEvents();
    }

    emit searchFinished(results.size());
    return results;
}
