// src/search/ProjectSearchEngine.h
#pragma once
#include <QObject>
#include <QString>
#include <QList>

struct ProjectSearchResult {
    QString filePath;
    int lineNumber;       // 1-based
    int columnNumber;     // 1-based
    QString lineText;      // the full matching line, for preview
};

class ProjectSearchEngine : public QObject {
    Q_OBJECT
public:
    explicit ProjectSearchEngine(QObject* parent = nullptr);

    void setProjectRoot(const QString& rootPath);
    void setFileFilters(const QStringList& extensions);

    QList<ProjectSearchResult> search(const QString& term, bool matchCase, bool wholeWord);

signals:
    void searchProgress(int filesScanned, int totalFiles);
    void searchFinished(int totalMatches);

private:
    QString m_projectRoot;
    QStringList m_fileFilters;

    QStringList collectProjectFiles() const;
    QList<ProjectSearchResult> searchFile(const QString& path, const QRegularExpression& pattern) const;
};
