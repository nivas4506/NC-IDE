// src/editor/FileTypeRegistry.cpp
#include "FileTypeRegistry.h"
#include <QFileInfo>

FileTypeInfo FileTypeRegistry::detect(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    
    FileTypeInfo info;
    info.language = Language::PlainText;
    info.languageName = "PlainText";
    info.buildCommand = "";
    info.runCommand = "";
    
    if (ext == "c") {
        info.language = Language::C;
        info.languageName = "C";
        info.buildCommand = "gcc -g -Wall \"${file}\" -o \"${fileDirname}/${fileBasenameNoExtension}\"";
        info.runCommand = "\"${fileDirname}/${fileBasenameNoExtension}\"";
    } else if (ext == "h") {
        info.language = Language::C;
        info.languageName = "C Header";
    } else if (ext == "cpp" || ext == "cc" || ext == "cxx") {
        info.language = Language::Cpp;
        info.languageName = "C++";
        info.buildCommand = "g++ -std=c++17 -g -Wall \"${file}\" -o \"${fileDirname}/${fileBasenameNoExtension}\"";
        info.runCommand = "\"${fileDirname}/${fileBasenameNoExtension}\"";
    } else if (ext == "hpp" || ext == "hh") {
        info.language = Language::Cpp;
        info.languageName = "C++ Header";
    } else if (ext == "py") {
        info.language = Language::Python;
        info.languageName = "Python";
        info.runCommand = "python \"${file}\"";
    } else if (ext == "java") {
        info.language = Language::Java;
        info.languageName = "Java";
        info.buildCommand = "javac \"${file}\"";
        info.runCommand = "java -cp \"${fileDirname}\" \"${fileBasenameNoExtension}\"";
    } else if (ext == "js" || ext == "jsx" || ext == "mjs") {
        info.language = Language::JavaScript;
        info.languageName = "JavaScript";
        info.runCommand = "node \"${file}\"";
    } else if (ext == "html" || ext == "htm") {
        info.language = Language::Html;
        info.languageName = "HTML";
        info.runCommand = "explorer.exe \"${file}\"";
    } else if (ext == "css") {
        info.language = Language::Css;
        info.languageName = "CSS";
    } else if (ext == "json") {
        info.language = Language::Json;
        info.languageName = "JSON";
    } else if (ext == "md" || ext == "markdown") {
        info.language = Language::Markdown;
        info.languageName = "Markdown";
    }
    
    return info;
}
