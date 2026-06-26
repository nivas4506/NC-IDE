// src/editor/FileTypeRegistry.h
#pragma once
#include <QString>
#include "../workbench/editor/SyntaxHighlighter.h"

struct FileTypeInfo {
    Language language;
    QString languageName;
    QString buildCommand;
    QString runCommand;
};

class FileTypeRegistry {
public:
    static FileTypeInfo detect(const QString& filePath);
};
