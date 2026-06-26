// src/language/AutoCompleter.cpp
#include "AutoCompleter.h"
#include "../workbench/editor/CodeEditor.h"
#include <QAbstractItemView>
#include <QScrollBar>

AutoCompleter::AutoCompleter(CodeEditor* editor)
    : QObject(editor), m_editor(editor), m_completer(new QCompleter(this)), m_model(new QStringListModel(this)), m_language(Language::PlainText) {
    
    setupCompleter();
    // m_editor->setCompleter(m_completer); // TODO: implement in CodeEditor
}

AutoCompleter::~AutoCompleter() {}

void AutoCompleter::setupCompleter() {
    m_completer->setModel(m_model);
    m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setWrapAround(false);
    
    // Style the popup to match Dracula theme
    QAbstractItemView* popup = m_completer->popup();
    popup->setStyleSheet(
        "QListView {"
        "  background-color: #282a36;"
        "  color: #f8f8f2;"
        "  border: 1px solid #44475a;"
        "  selection-background-color: #44475a;"
        "}"
    );
}

void AutoCompleter::setLanguage(Language lang) {
    m_language = lang;
    m_model->setStringList(getKeywordsForLanguage(lang));
}

void AutoCompleter::updateCompletionModel(const QString& prefix) {
    m_completer->setCompletionPrefix(prefix);
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
}

QStringList AutoCompleter::getKeywordsForLanguage(Language lang) const {
    switch (lang) {
        case Language::Cpp:
        case Language::C:
            return QStringList{
                "auto", "break", "case", "char", "class", "const", "continue", "default",
                "do", "double", "else", "enum", "extern", "float", "for", "goto", "if",
                "int", "long", "namespace", "return", "short", "signed", "sizeof", "static",
                "struct", "switch", "template", "typedef", "union", "unsigned", "void", "volatile", "while",
                "std", "cout", "endl", "vector", "string", "map", "set", "include", "define"
            };
        case Language::Python:
            return QStringList{
                "False", "None", "True", "and", "as", "assert", "async", "await", "break",
                "class", "continue", "def", "del", "elif", "else", "except", "finally",
                "for", "from", "global", "if", "import", "in", "is", "lambda", "nonlocal",
                "not", "or", "pass", "raise", "return", "try", "while", "with", "yield",
                "print", "len", "range", "self"
            };
        case Language::Java:
            return QStringList{
                "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
                "class", "const", "continue", "default", "do", "double", "else", "enum",
                "extends", "final", "finally", "float", "for", "goto", "if", "implements",
                "import", "instanceof", "int", "interface", "long", "native", "new", "package",
                "private", "protected", "public", "return", "short", "static", "strictfp",
                "super", "switch", "synchronized", "this", "throw", "throws", "transient",
                "try", "void", "volatile", "while", "String", "System", "out", "println"
            };
        case Language::JavaScript:
            return QStringList{
                "break", "case", "catch", "class", "const", "continue", "debugger", "default",
                "delete", "do", "else", "export", "extends", "finally", "for", "function",
                "if", "import", "in", "instanceof", "new", "return", "super", "switch",
                "this", "throw", "try", "typeof", "var", "void", "while", "with", "yield",
                "let", "await", "async", "console", "log", "document", "window"
            };
        default:
            return QStringList();
    }
}
