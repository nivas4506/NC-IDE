// src/extensions/ExtensionAPI.h
#pragma once
#include <QString>
#include <functional>

class QWidget;
class CodeEditor;

class ExtensionAPI {
public:
    virtual ~ExtensionAPI() = default;

    // Command Registration
    virtual void registerCommand(const QString& id, const QString& label, std::function<void()> callback) = 0;

    // UI Integration
    virtual void registerSidebarView(const QString& id, const QString& title, QWidget* widget) = 0;
    virtual void showNotification(const QString& message, const QString& type = "info") = 0;

    // Editor Access
    virtual CodeEditor* getActiveEditor() = 0;

    // Workspace Access
    virtual QString getWorkspaceRoot() = 0;
};
