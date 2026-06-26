// src/language/CodeFormatter.cpp
#include "CodeFormatter.h"
#include <QStringList>
#include <QJsonDocument>

QString CodeFormatter::format(const QString& text, Language lang) {
    switch (lang) {
        case Language::C:
        case Language::Cpp:
        case Language::Java:
        case Language::JavaScript:
            return formatCpp(text); // Basic brace language formatting
        case Language::Python:
            return formatPython(text);
        case Language::Json:
            return formatJson(text);
        default:
            return text; // No formatting for plain text
    }
}

// Very rudimentary formatting for brace languages
QString CodeFormatter::formatCpp(const QString& text) {
    QStringList lines = text.split('\n');
    QString result;
    int indentLevel = 0;
    const QString indentString = "    "; // 4 spaces

    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty()) {
            result += "\n";
            continue;
        }

        if (line.startsWith('}')) {
            indentLevel = std::max(0, indentLevel - 1);
        }

        for (int i = 0; i < indentLevel; ++i) {
            result += indentString;
        }
        result += line + "\n";

        if (line.endsWith('{')) {
            indentLevel++;
        }
    }

    return result.trimmed(); // Remove trailing newline
}

// Python mostly just preserves structure but cleans up whitespace
QString CodeFormatter::formatPython(const QString& text) {
    QStringList lines = text.split('\n');
    QString result;
    
    for (QString line : lines) {
        // Strip trailing whitespace
        while (line.endsWith(' ') || line.endsWith('\t')) {
            line.chop(1);
        }
        result += line + "\n";
    }
    return result.trimmed();
}

QString CodeFormatter::formatJson(const QString& text) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError) {
        return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    }
    return text; // If invalid JSON, leave as is
}
