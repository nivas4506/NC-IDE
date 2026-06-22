// src/tabs/TabManager.h
#pragma once
#include <QObject>
#include <QTabWidget>
#include <QMap>
#include "EditorTab.h"
#include "../editor/CodeEditor.h"

class TabManager : public QObject {
    Q_OBJECT
public:
    explicit TabManager(QTabWidget* tabWidget, QObject* parent = nullptr);

    CodeEditor* openFile(const QString& filePath);   // returns existing tab's editor if already open
    CodeEditor* newUntitledTab();
    bool closeTab(int index, bool promptIfDirty = true);
    void closeAllTabs(bool promptIfDirty = true);
    void closeOthers(int keepIndex);

    bool saveTab(int index);
    bool saveTabAs(int index, const QString& newPath);
    bool saveAll();

    CodeEditor* currentEditor() const;
    CodeEditor* editorAt(int index) const;
    QString filePathAt(int index) const;
    int indexOfFile(const QString& filePath) const;
    int tabCount() const;

    QList<EditorTab> allOpenTabs() const;     // for session persistence

signals:
    void tabDirtyStateChanged(int index, bool isDirty);
    void allTabsClosed();
    void activeFileChanged(const QString& filePath);

private slots:
    void onTabCloseRequested(int index);
    void onTabMoved(int from, int to);
    void onCurrentTabChanged(int index);
    void onEditorModified();

private:
    QTabWidget* m_tabWidget;
    QMap<int, EditorTab> m_tabData;
    int m_untitledCounter = 1;

    Language detectLanguageFromExtension(const QString& path) const;
    QIcon dirtyIndicatorIcon() const;     // small dot icon shown in tab title when unsaved
    void updateTabTitle(int index);
    bool promptSaveDiscardCancel(int index); // returns true = proceed with close
};
