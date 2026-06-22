// src/MainWindow.h
#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QDockWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QMenu>
#include "tabs/TabManager.h"
#include "search/SearchManager.h"
#include "search/ProjectSearchEngine.h"
#include "session/SessionManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // File Actions
    void onNewFile();
    void onOpenFile();
    void onOpenProject();
    void onSave();
    void onSaveAs();
    void onSaveAll();
    void onCloseTab();
    void onCloseAll();
    void onRecentFileTriggered();
    
    // Edit Actions
    void onUndo();
    void onRedo();
    void onGoToLine();
    
    // Settings Actions
    void onShowPreferences();

    // Search Actions
    void onToggleFindPanel(bool replaceMode);
    void onFindNext();
    void onFindPrev();
    void onReplaceCurrent();
    void onReplaceAll();
    void onProjectSearch();
    void onProjectSearchResultDoubleClicked(QTreeWidgetItem* item, int column);
    
    // Status Bar & Tab updates
    void updateStatusBar(const QString& filePath);
    void onActiveFileChanged(const QString& filePath);
    void onCursorPositionChanged(const QString& label);

private:
    void createMenus();
    void createToolBar();
    void createStatusBarWidget();
    void createFindPanel();
    void createProjectSearchDock();
    void updateRecentFilesMenu();

    // Core Managers
    TabManager* m_tabManager;
    SearchManager* m_searchManager;
    ProjectSearchEngine* m_projectSearchEngine;
    SessionManager* m_sessionManager;

    // UI Widgets
    QTabWidget* m_tabWidget;
    
    // Recent Files Menu
    QMenu* m_recentFilesMenu;

    // Find & Replace Panel (Inline at bottom)
    QWidget* m_findPanel;
    QLineEdit* m_findInput;
    QLineEdit* m_replaceInput;
    QPushButton* m_findNextBtn;
    QPushButton* m_findPrevBtn;
    QPushButton* m_replaceBtn;
    QPushButton* m_replaceAllBtn;
    QCheckBox* m_matchCaseCheck;
    QCheckBox* m_wholeWordCheck;
    QWidget* m_replaceRowWidget;

    // Project Search Dock
    QDockWidget* m_projectSearchDock;
    QLineEdit* m_projectSearchInput;
    QTreeWidget* m_projectSearchResults;
    QPushButton* m_projectSearchBtn;
    QCheckBox* m_projMatchCaseCheck;
    QCheckBox* m_projWholeWordCheck;

    // Status bar widgets
    QLabel* m_statusFilePath;
    QLabel* m_statusCursorPos;
    QLabel* m_statusLanguage;
};
