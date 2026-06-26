// src/problems/ProblemParser.h
#pragma once
#include <QString>
#include <QVector>

struct Diagnostic {
    QString filePath;
    int line; // 1-based
    int column; // 1-based, 0 if not available
    QString severity; // "Error" | "Warning" | "Info"
    QString message;
    QString source; // "Build" | "Runtime"
};

class ProblemParser {
public:
    static QVector<Diagnostic> parseBuildOutput(const QString& output);
    static QVector<Diagnostic> parseRuntimeOutput(const QString& output);
};
