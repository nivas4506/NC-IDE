// src/problems/ProblemParser.cpp
#include "ProblemParser.h"
#include <QRegularExpression>
#include <QStringList>
#include <QFileInfo>

QVector<Diagnostic> ProblemParser::parseBuildOutput(const QString& output) {
    QVector<Diagnostic> diagnostics;
    QStringList lines = output.split('\n');
    
    QRegularExpression gccRegex(R"(^(.*?):(\d+):(?:(\d+):)?\s*(error|warning|info|note):\s*(.*)$)", QRegularExpression::CaseInsensitiveOption);
    
    for (const QString& line : lines) {
        QString cleanLine = line.trimmed();
        QRegularExpressionMatch match = gccRegex.match(cleanLine);
        if (match.hasMatch()) {
            Diagnostic diag;
            diag.filePath = match.captured(1).trimmed();
            diag.line = match.captured(2).toInt();
            diag.column = match.captured(3).isEmpty() ? 0 : match.captured(3).toInt();
            
            QString sev = match.captured(4).toLower();
            if (sev == "error") diag.severity = "Error";
            else if (sev == "warning") diag.severity = "Warning";
            else diag.severity = "Info";
            
            diag.message = match.captured(5).trimmed();
            diag.source = "Build";
            
            diagnostics.append(diag);
        }
    }
    
    return diagnostics;
}

QVector<Diagnostic> ProblemParser::parseRuntimeOutput(const QString& output) {
    QVector<Diagnostic> diagnostics;
    QStringList lines = output.split('\n');
    
    QRegularExpression pyFrameRegex(R"raw(^\s*File\s+"([^"]+)",\s*line\s+(\d+)(?:,\s*in\s+(.+))?$)raw");
    QRegularExpression nodeRegex(R"raw(^\s*at\s+(?:[^\(]*\((.*?):(\d+):(\d+)\)|(.*?):(\d+):(\d+))\s*$)raw");
    
    QString lastPyFile;
    int lastPyLine = 0;
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        QString cleanLine = line.trimmed();
        
        // Try Python traceback frame
        QRegularExpressionMatch pyMatch = pyFrameRegex.match(line);
        if (pyMatch.hasMatch()) {
            lastPyFile = pyMatch.captured(1).trimmed();
            lastPyLine = pyMatch.captured(2).toInt();
            continue;
        }
        
        // If we have a Python frame and we hit an error message line (like "NameError: ...")
        if (!lastPyFile.isEmpty() && lastPyLine > 0 && cleanLine.contains(':') && !cleanLine.startsWith("File ") && !cleanLine.startsWith("Traceback")) {
            // Check if this looks like a Python exception line
            // Python exception lines usually have the class name and message separated by colon, e.g. "ValueError: invalid literal"
            // Ensure it's not a line of code (indented)
            if (!line.startsWith("    ") && !line.startsWith("\t")) {
                Diagnostic diag;
                diag.filePath = lastPyFile;
                diag.line = lastPyLine;
                diag.column = 0;
                diag.severity = "Error";
                diag.message = cleanLine;
                diag.source = "Runtime";
                diagnostics.append(diag);
                
                // Reset to avoid double matching
                lastPyFile.clear();
                lastPyLine = 0;
            }
        }
        
        // Try Node.js traceback frame
        QRegularExpressionMatch nodeMatch = nodeRegex.match(line);
        if (nodeMatch.hasMatch()) {
            QString path = nodeMatch.captured(1).isEmpty() ? nodeMatch.captured(4) : nodeMatch.captured(1);
            int ln = nodeMatch.captured(2).isEmpty() ? nodeMatch.captured(5).toInt() : nodeMatch.captured(2).toInt();
            int col = nodeMatch.captured(3).isEmpty() ? nodeMatch.captured(6).toInt() : nodeMatch.captured(3).toInt();
            
            path = path.trimmed();
            
            // Skip node internal libraries or external packages
            if (!path.startsWith("node:") && !path.contains("node_modules") && !path.contains("internal/")) {
                Diagnostic diag;
                diag.filePath = path;
                diag.line = ln;
                diag.column = col;
                diag.severity = "Error";
                
                // Message might be on one of the previous lines (first line of the Node.js crash is usually the error message)
                QString msg = "Uncaught Exception";
                for (int j = qMax(0, i - 5); j < i; ++j) {
                    QString prevLine = lines[j].trimmed();
                    if (prevLine.contains("Error:") || prevLine.contains("Exception:")) {
                        msg = prevLine;
                        break;
                    }
                }
                diag.message = msg;
                diag.source = "Runtime";
                
                diagnostics.append(diag);
                
                // Only take the first application-level frame
                break;
            }
        }
    }
    
    return diagnostics;
}
