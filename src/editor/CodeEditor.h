// src/editor/CodeEditor.h
#pragma once
#include <QPlainTextEdit>
#include <QColor>
#include "SyntaxHighlighter.h"

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget* parent = nullptr);

    void setLanguage(Language lang);
    Language language() const;

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth() const;

    // Editor enhancements
    void duplicateLine();
    void deleteLine();
    void moveLineUp();
    void moveLineDown();
    void goToLine(int lineNumber);

    // Dirty-state tracking for tab indicators
    bool isModifiedSinceSave() const { return document()->isModified(); }

    // Cursor position display ("Ln 12, Col 4")
    QString cursorPositionLabel() const;

signals:
    void cursorPositionChanged2(const QString& label); // forwarded to status bar

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);
    void matchBrackets();

private:
    LineNumberArea* m_lineNumberArea;
    SyntaxHighlighter* m_highlighter;
    Language m_language = Language::PlainText;

    QColor m_backgroundColor;
    QColor m_foregroundColor;
    QColor m_lineHighlightColor;
    QColor m_lineNumberColor;
    QColor m_lineNumberActiveColor;
    QColor m_selectionColor;

    void highlightMatchingBracket(QTextCursor cursor);
    void applyThemeColors(const QString& themeName);
    static const QString BRACKET_PAIRS; // "()[]{}"
};
