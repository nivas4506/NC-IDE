// src/editor/SyntaxHighlighter.cpp
#include "SyntaxHighlighter.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent, Language lang)
    : QSyntaxHighlighter(parent), m_language(lang) {
    
    // Default multi-line comment delimiters for C-style languages
    m_commentStartExpr = QRegularExpression(R"(/\*)");
    m_commentEndExpr = QRegularExpression(R"(\*/)");
    
    // Python triple quotes
    m_tripleQuoteExpr = QRegularExpression(R"('''|""")");
    
    applyTheme("dark");
}

void SyntaxHighlighter::setLanguage(Language lang) {
    m_language = lang;
    buildRulesForLanguage();
    rehighlight();
}

void QCharFormatFromJson(const QJsonValue& val, QTextCharFormat& format) {
    if (val.isObject()) {
        QJsonObject obj = val.toObject();
        if (obj.contains("color")) {
            format.setForeground(QColor(obj.value("color").toString()));
        }
        if (obj.contains("bold") && obj.value("bold").toBool()) {
            format.setFontWeight(QFont::Bold);
        }
        if (obj.contains("italic") && obj.value("italic").toBool()) {
            format.setFontItalic(true);
        }
    }
}

void SyntaxHighlighter::applyTheme(const QString& themeName) {
    // 1. Initialize safe fallback dark-theme defaults
    m_keywordFormat.setForeground(QColor("#FF79C6"));
    m_keywordFormat.setFontWeight(QFont::Bold);
    m_typeFormat.setForeground(QColor("#8BE9FD"));
    m_typeFormat.setFontItalic(true);
    m_stringFormat.setForeground(QColor("#F1FA8C"));
    m_commentFormat.setForeground(QColor("#6272A4"));
    m_commentFormat.setFontItalic(true);
    m_functionFormat.setForeground(QColor("#50FA7B"));
    m_numberFormat.setForeground(QColor("#BD93F9"));
    m_preprocessorFormat.setForeground(QColor("#FFB86C"));

    if (themeName == "light") {
        m_keywordFormat.setForeground(QColor("#0000FF"));
        m_keywordFormat.setFontWeight(QFont::Bold);
        m_typeFormat.setForeground(QColor("#008080"));
        m_typeFormat.setFontItalic(true);
        m_stringFormat.setForeground(QColor("#A31515"));
        m_commentFormat.setForeground(QColor("#008000"));
        m_commentFormat.setFontItalic(true);
        m_functionFormat.setForeground(QColor("#795E26"));
        m_numberFormat.setForeground(QColor("#098658"));
        m_preprocessorFormat.setForeground(QColor("#A31515"));
    }

    // 2. Try loading custom JSON theme file
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/resources/themes/" + themeName + ".json",
        QCoreApplication::applicationDirPath() + "/themes/" + themeName + ".json",
        QDir::currentPath() + "/resources/themes/" + themeName + ".json",
        QDir::currentPath() + "/themes/" + themeName + ".json"
    };

    QFile file;
    for (const QString& path : searchPaths) {
        file.setFileName(path);
        if (file.open(QIODevice::ReadOnly)) {
            break;
        }
    }

    if (file.isOpen()) {
        QByteArray data = file.readAll();
        file.close();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (rootObj.contains("syntax")) {
                QJsonObject syntaxObj = rootObj.value("syntax").toObject();
                QCharFormatFromJson(syntaxObj.value("keyword"), m_keywordFormat);
                QCharFormatFromJson(syntaxObj.value("type"), m_typeFormat);
                QCharFormatFromJson(syntaxObj.value("string"), m_stringFormat);
                QCharFormatFromJson(syntaxObj.value("comment"), m_commentFormat);
                QCharFormatFromJson(syntaxObj.value("function"), m_functionFormat);
                QCharFormatFromJson(syntaxObj.value("number"), m_numberFormat);
                QCharFormatFromJson(syntaxObj.value("preprocessor"), m_preprocessorFormat);
            }
        }
    }

    buildRulesForLanguage();
    rehighlight();
}

void SyntaxHighlighter::addRule(const QString& pattern, const QTextCharFormat& fmt) {
    HighlightRule rule;
    rule.pattern = QRegularExpression(pattern);
    rule.format = fmt;
    m_rules.push_back(rule);
}

void SyntaxHighlighter::buildRulesForLanguage() {
    m_rules.clear();

    if (m_language == Language::PlainText) {
        return;
    }

    QStringList keywords;
    QStringList types;
    QString commentPattern;
    QString preprocessorPattern;

    switch (m_language) {
        case Language::C:
            keywords = {"const", "static", "struct", "union", "enum", "typedef", "sizeof", "if", "else", 
                        "for", "while", "do", "switch", "case", "break", "continue", "return", "goto", "extern", "volatile"};
            types = {"int", "char", "float", "double", "void", "short", "long", "unsigned", "signed"};
            commentPattern = R"(//[^\n]*)";
            preprocessorPattern = R"(#[a-zA-Z_]+)";
            break;

        case Language::Cpp:
            keywords = {"const", "static", "struct", "union", "enum", "typedef", "sizeof", "if", "else", 
                        "for", "while", "do", "switch", "case", "break", "continue", "return", "goto", "extern", "volatile",
                        "class", "public", "private", "protected", "virtual", "override", "new", "delete", "this", 
                        "namespace", "template", "typename", "using", "try", "catch", "throw", "nullptr", "bool", 
                        "true", "false", "auto", "constexpr", "noexcept", "friend", "inline", "explicit", "operator"};
            types = {"int", "char", "float", "double", "void", "short", "long", "unsigned", "signed", "size_t", "bool", "std::[a-zA-Z0-9_]+"};
            commentPattern = R"(//[^\n]*)";
            preprocessorPattern = R"(#[a-zA-Z_]+)";
            break;

        case Language::Python:
            keywords = {"def", "class", "import", "from", "as", "if", "elif", "else", "for", "while", 
                        "try", "except", "finally", "with", "return", "yield", "lambda", "pass", "break", 
                        "continue", "global", "nonlocal", "assert", "del", "raise", "in", "is", "not", "and", "or", 
                        "None", "True", "False", "self"};
            types = {"int", "float", "str", "bool", "list", "dict", "tuple", "set", "object"};
            commentPattern = R"(#[^\n]*)";
            break;

        case Language::Java:
            keywords = {"public", "private", "protected", "class", "interface", "extends", "implements", 
                        "static", "final", "void", "if", "else", "for", "while", "do", "switch", "case", 
                        "break", "continue", "return", "new", "this", "super", "try", "catch", "finally", 
                        "throw", "throws", "import", "package", "abstract", "synchronized", "volatile", 
                        "transient", "instanceof"};
            types = {"int", "double", "float", "boolean", "char", "long", "short", "byte", "String"};
            commentPattern = R"(//[^\n]*)";
            break;

        case Language::JavaScript:
            keywords = {"function", "var", "let", "const", "if", "else", "for", "while", "do", "switch", 
                        "case", "break", "continue", "return", "new", "this", "class", "extends", "super", 
                        "try", "catch", "finally", "throw", "typeof", "instanceof", "in", "of", "async", 
                        "await", "import", "export", "default", "null", "undefined", "true", "false"};
            types = {"var", "let", "const", "Array", "Object", "Number", "String", "Boolean"};
            commentPattern = R"(//[^\n]*)";
            break;

        case Language::Html:
            // Tags: e.g. <html>, </div>
            addRule(R"(<\/?[a-zA-Z][a-zA-Z0-9]*\b)", m_keywordFormat);
            // Attributes
            addRule(R"(\b[a-zA-Z\-]+(?=\s*=))", m_typeFormat);
            // String Values
            addRule(R"("[^"\\]*(?:\\.[^"\\]*)*")", m_stringFormat);
            addRule(R"('[^'\\]*(?:\\.[^'\\]*)*')", m_stringFormat);
            // Comments
            addRule(R"(<!--[^\n]*-->)", m_commentFormat);
            return;

        case Language::Css:
            // selectors: e.g. .class, #id, tag
            addRule(R"([\.\#]?[a-zA-Z0-9\-\_]+(?=\s*\{))", m_functionFormat);
            // property names: e.g. color, border-radius
            addRule(R"(\b[a-zA-Z\-]+(?=\s*:))", m_typeFormat);
            // values and units: e.g. 10px, 1.5em, #fff
            addRule(R"(#[0-9a-fA-F]{3,6}|\b[0-9]+(?:\.[0-9]+)?(?:px|em|rem|%|vh|vw|ms|s)?\b)", m_numberFormat);
            // !important
            addRule(R"(!important\b)", m_keywordFormat);
            // Comments
            commentPattern = R"(/\*[^*]*\*+(?:[^/*][^*]*\*+)*/)";
            break;

        default:
            break;
    }

    // Add preprocessor rule first
    if (!preprocessorPattern.isEmpty()) {
        addRule(preprocessorPattern, m_preprocessorFormat);
    }

    // Add keyword rules
    for (const QString& kw : keywords) {
        addRule("\\b" + kw + "\\b", m_keywordFormat);
    }

    // Add type rules
    for (const QString& ty : types) {
        addRule("\\b" + ty + "\\b", m_typeFormat);
    }

    // Function names
    addRule(R"(\b[A-Za-z_][A-Za-z0-9_]*(?=\s*\())", m_functionFormat);

    // Numbers
    addRule(R"(\b0[xX][0-9a-fA-F]+\b|\b[0-9]+(?:\.[0-9]+)?(?:[eE][-+]?[0-9]+)?[fFLuU]?\b)", m_numberFormat);

    // Double quoted strings
    addRule(R"("[^"\\]*(?:\\.[^"\\]*)*")", m_stringFormat);

    // Single quoted strings (if not Python, where it's handled differently)
    if (m_language != Language::Python) {
        addRule(R"('[^'\\]*(?:\\.[^'\\]*)*')", m_stringFormat);
    }

    // Single-line comment
    if (!commentPattern.isEmpty()) {
        addRule(commentPattern, m_commentFormat);
    }
}

void SyntaxHighlighter::highlightBlock(const QString& text) {
    // 1. Standard regex rules
    for (const HighlightRule& rule : m_rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(Normal);

    // 2. Multiline formatting
    if (m_language == Language::C || m_language == Language::Cpp || m_language == Language::Java || m_language == Language::JavaScript || m_language == Language::Css) {
        highlightMultilineComments(text);
    } else if (m_language == Language::Python) {
        highlightMultilineStrings(text);
    }
}

void SyntaxHighlighter::highlightMultilineComments(const QString& text) {
    int startIndex = 0;
    if (previousBlockState() != InComment) {
        QRegularExpressionMatch match = m_commentStartExpr.match(text);
        startIndex = match.capturedStart();
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch matchEnd = m_commentEndExpr.match(text, startIndex);
        int endIndex = matchEnd.capturedStart();
        int commentLength = 0;
        if (endIndex < 0) {
            setCurrentBlockState(InComment);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + matchEnd.capturedLength();
        }
        setFormat(startIndex, commentLength, m_commentFormat);
        startIndex = m_commentStartExpr.match(text, startIndex + commentLength).capturedStart();
    }
}

void SyntaxHighlighter::highlightMultilineStrings(const QString& text) {
    int startIndex = 0;
    if (previousBlockState() != InTripleString) {
        QRegularExpressionMatch match = m_tripleQuoteExpr.match(text);
        startIndex = match.capturedStart();
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch matchEnd = m_tripleQuoteExpr.match(text, startIndex + 3);
        int endIndex = matchEnd.capturedStart();
        int stringLength = 0;
        if (endIndex < 0) {
            setCurrentBlockState(InTripleString);
            stringLength = text.length() - startIndex;
        } else {
            stringLength = endIndex - startIndex + matchEnd.capturedLength();
        }
        setFormat(startIndex, stringLength, m_stringFormat);
        startIndex = m_tripleQuoteExpr.match(text, startIndex + stringLength).capturedStart();
    }
}
