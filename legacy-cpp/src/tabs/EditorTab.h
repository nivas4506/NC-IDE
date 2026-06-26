// src/tabs/EditorTab.h
#pragma once
#include <QString>

struct EditorTab {
    QString filePath;      // empty if unsaved/new
    QString displayName;   // filename or "Untitled-N"
    bool isDirty = false;
    bool isNewFile = true;
};
