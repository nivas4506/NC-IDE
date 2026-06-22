// src/search/SearchManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QTextCursor>
#include "../editor/CodeEditor.h"

struct SearchOptions {
    bool matchCase = false;
    bool wholeWord = false;
    bool searchBackward = false;
};

class SearchManager : public QObject {
    Q_OBJECT
public:
    explicit SearchManager(QObject* parent = nullptr);

    void setActiveEditor(CodeEditor* editor);

    // Returns true if a match was found and selected in the editor
    bool findNext(const QString& term, const SearchOptions& opts);
    bool findPrevious(const QString& term, const SearchOptions& opts);

    int replaceCurrent(const QString& term, const QString& replacement, const SearchOptions& opts);
    int replaceAll(const QString& term, const QString& replacement, const SearchOptions& opts);

    int countMatches(const QString& term, const SearchOptions& opts) const;

signals:
    void searchWrapped();
    void noMatchFound();

private:
    CodeEditor* m_activeEditor = nullptr;
    QTextDocument::FindFlags buildFindFlags(const SearchOptions& opts, bool backward) const;
};
