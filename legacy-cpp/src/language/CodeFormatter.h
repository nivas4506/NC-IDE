// src/language/CodeFormatter.h
#pragma once
#include <QString>
#include "../workbench/editor/SyntaxHighlighter.h"

class CodeFormatter {
public:
    static QString format(const QString& text, Language lang);

private:
    static QString formatCpp(const QString& text);
    static QString formatPython(const QString& text);
    static QString formatJson(const QString& text);
};
