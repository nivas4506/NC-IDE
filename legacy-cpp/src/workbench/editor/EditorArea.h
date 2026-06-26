// src/workbench/editor/EditorArea.h
#pragma once
#include <QWidget>
#include <QSplitter>
#include <QVector>
#include "EditorGroup.h"

class EditorArea : public QWidget {
    Q_OBJECT
public:
    explicit EditorArea(QWidget* parent = nullptr);
    ~EditorArea();

    EditorGroup* activeGroup() const { return m_activeGroup; }
    QVector<EditorGroup*> groups() const { return m_groups; }

    CodeEditor* currentEditor() const;
    QString currentFilePath() const;

    CodeEditor* openFile(const QString& filePath);
    CodeEditor* newUntitledTab();
    bool closeCurrentTab();
    void closeAll();
    bool saveCurrentFile();
    bool saveAll();

    void split(Qt::Orientation orientation);
    void setProjectRoot(const QString& root);

signals:
    void activeFileChanged(const QString& filePath);
    void activeEditorChanged(CodeEditor* editor);

private slots:
    void handleGroupFocused(EditorGroup* group);
    void handleGroupFileChanged(const QString& path);
    void handleGroupAllTabsClosed();

private:
    QSplitter* m_mainSplitter;
    QVector<EditorGroup*> m_groups;
    EditorGroup* m_activeGroup = nullptr;
    QString m_projectRoot;

    void addGroup(EditorGroup* group);
    void removeGroup(EditorGroup* group);
    void setActiveGroup(EditorGroup* group);
};
