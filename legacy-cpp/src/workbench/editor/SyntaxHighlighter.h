// src/editor/SyntaxHighlighter.h
#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <vector>

enum class Language {
    PlainText, C, Cpp, Python, Java, JavaScript, Html, Css, Json, Markdown
};

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument* parent, Language lang);

    void setLanguage(Language lang);
    Language language() const { return m_language; }

    // Applies theme colors (called by SettingsManager on theme change)
    void applyTheme(const QString& themeName);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void buildRulesForLanguage();
    void addRule(const QString& pattern, const QTextCharFormat& fmt);
    void highlightMultilineComments(const QString& text);
    void highlightMultilineStrings(const QString& text); // Python triple-quoted strings

    Language m_language;
    std::vector<HighlightRule> m_rules;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_preprocessorFormat;   // #include, #define (C/C++)

    QRegularExpression m_commentStartExpr;  // /* for C-family
    QRegularExpression m_commentEndExpr;    // */
    QRegularExpression m_tripleQuoteExpr;   // ''' or """ for Python

    // Block states for multiline constructs
    enum BlockState { Normal = 0, InComment = 1, InTripleString = 2 };
};
