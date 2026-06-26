// src/search/SearchManager.cpp
#include "SearchManager.h"
#include <QDebug>

SearchManager::SearchManager(QObject* parent)
    : QObject(parent) {
}

void SearchManager::setActiveEditor(CodeEditor* editor) {
    m_activeEditor = editor;
}

QTextDocument::FindFlags SearchManager::buildFindFlags(const SearchOptions& opts, bool backward) const {
    QTextDocument::FindFlags flags;
    if (opts.matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (opts.wholeWord) flags |= QTextDocument::FindWholeWords;
    if (backward) flags |= QTextDocument::FindBackward;
    return flags;
}

bool SearchManager::findNext(const QString& term, const SearchOptions& opts) {
    if (!m_activeEditor || term.isEmpty()) return false;

    QTextDocument::FindFlags flags = buildFindFlags(opts, false);
    QTextCursor currentCursor = m_activeEditor->textCursor();
    
    QTextCursor foundCursor = m_activeEditor->document()->find(term, currentCursor, flags);
    if (!foundCursor.isNull()) {
        m_activeEditor->setTextCursor(foundCursor);
        return true;
    }

    // Wrap around to start
    QTextCursor startCursor(m_activeEditor->document());
    startCursor.setPosition(0);
    foundCursor = m_activeEditor->document()->find(term, startCursor, flags);
    if (!foundCursor.isNull()) {
        m_activeEditor->setTextCursor(foundCursor);
        emit searchWrapped();
        return true;
    }

    emit noMatchFound();
    return false;
}

bool SearchManager::findPrevious(const QString& term, const SearchOptions& opts) {
    if (!m_activeEditor || term.isEmpty()) return false;

    QTextDocument::FindFlags flags = buildFindFlags(opts, true);
    QTextCursor currentCursor = m_activeEditor->textCursor();
    
    // In Qt document find, if cursor has a selection, searching backward starts from selection start
    if (currentCursor.hasSelection()) {
        currentCursor.setPosition(currentCursor.selectionStart());
    }

    QTextCursor foundCursor = m_activeEditor->document()->find(term, currentCursor, flags);
    if (!foundCursor.isNull()) {
        m_activeEditor->setTextCursor(foundCursor);
        return true;
    }

    // Wrap around to end
    QTextCursor endCursor(m_activeEditor->document());
    endCursor.setPosition(m_activeEditor->document()->characterCount() - 1);
    foundCursor = m_activeEditor->document()->find(term, endCursor, flags);
    if (!foundCursor.isNull()) {
        m_activeEditor->setTextCursor(foundCursor);
        emit searchWrapped();
        return true;
    }

    emit noMatchFound();
    return false;
}

int SearchManager::replaceCurrent(const QString& term, const QString& replacement, const SearchOptions& opts) {
    if (!m_activeEditor || term.isEmpty()) return 0;

    QTextCursor cursor = m_activeEditor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText().compare(term, opts.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0) {
        cursor.insertText(replacement);
        findNext(term, opts);
        return 1;
    }

    findNext(term, opts);
    return 0;
}

int SearchManager::replaceAll(const QString& term, const QString& replacement, const SearchOptions& opts) {
    if (!m_activeEditor || term.isEmpty()) return 0;

    QTextCursor cursor(m_activeEditor->document());
    cursor.beginEditBlock();
    int count = 0;
    QTextDocument::FindFlags flags = buildFindFlags(opts, false);

    QTextCursor foundCursor = m_activeEditor->document()->find(term, cursor, flags);
    while (!foundCursor.isNull()) {
        foundCursor.insertText(replacement);
        count++;
        // Continue searching after replacement
        foundCursor = m_activeEditor->document()->find(term, foundCursor, flags);
    }
    cursor.endEditBlock();
    return count;
}

int SearchManager::countMatches(const QString& term, const SearchOptions& opts) const {
    if (!m_activeEditor || term.isEmpty()) return 0;

    int count = 0;
    QTextDocument::FindFlags flags = buildFindFlags(opts, false);
    QTextCursor cursor(m_activeEditor->document());
    
    QTextCursor foundCursor = m_activeEditor->document()->find(term, cursor, flags);
    while (!foundCursor.isNull()) {
        count++;
        // Move to end of found selection to scan next
        foundCursor.setPosition(foundCursor.position());
        foundCursor = m_activeEditor->document()->find(term, foundCursor, flags);
    }
    return count;
}
