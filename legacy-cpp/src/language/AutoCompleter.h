// src/language/AutoCompleter.h
#pragma once
#include <QObject>
#include <QCompleter>
#include <QStringListModel>
#include "../workbench/editor/SyntaxHighlighter.h" // For Language enum

class CodeEditor;

class AutoCompleter : public QObject {
    Q_OBJECT
public:
    explicit AutoCompleter(CodeEditor* editor);
    ~AutoCompleter();

    void setLanguage(Language lang);
    void updateCompletionModel(const QString& prefix);

private:
    CodeEditor* m_editor;
    QCompleter* m_completer;
    QStringListModel* m_model;
    Language m_language;

    QStringList getKeywordsForLanguage(Language lang) const;
    void setupCompleter();
};
