// src/workbench/editor/EditorGroup.h
#pragma once
#include <QWidget>
#include <QTabWidget>
#include "BreadcrumbBar.h"
#include "../../tabs/TabManager.h"

class EditorGroup : public QWidget {
    Q_OBJECT
public:
    explicit EditorGroup(QWidget* parent = nullptr);
    ~EditorGroup();

    TabManager* tabManager() const { return m_tabManager; }
    QTabWidget* tabWidget() const { return m_tabWidget; }
    BreadcrumbBar* breadcrumbBar() const { return m_breadcrumbBar; }

    CodeEditor* currentEditor() const;
    QString currentFilePath() const;

    void setProjectRoot(const QString& root);
    void setActive(bool active);

signals:
    void activeFileChanged(const QString& filePath);
    void editorGroupFocused(EditorGroup* group);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onActiveFileChanged(const QString& path);
    void onTabChanged(int index);

private:
    QTabWidget* m_tabWidget;
    TabManager* m_tabManager;
    BreadcrumbBar* m_breadcrumbBar;
    QString m_projectRoot;
    bool m_isActive = false;
};
