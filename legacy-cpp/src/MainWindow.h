// src/MainWindow.h
#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <QMap>
#include <QVector>
#include <QJsonArray>

#include "workbench/activitybar/ActivityBar.h"
#include "workbench/sidebar/Sidebar.h"
#include "workbench/editor/EditorArea.h"
#include "workbench/panels/BottomPanel.h"
#include "workbench/statusbar/StatusBar.h"
#include "workbench/CommandPalette.h"
#include "workbench/editor/CodeEditor.h"

#include "explorer/ExplorerView.h"
#include "search/SearchView.h"
#include "scm/SourceControlView.h"
#include "debug/RunDebugView.h"
#include "extensions/ExtensionsView.h"

#include "scm/GitManager.h"
#include "debug/BuildManager.h"
#include "debug/RunManager.h"
#include "debug/DebugManager.h"
#include "ai/AiAssistant.h"
#include "ai/AiPanel.h"
#include "workspace/WorkspaceManager.h"
#include "extensions/ExtensionManager.h"
#include "language/LspClient.h"

struct RunConfig {
    QString name;
    QString language;
    QString buildCommand;
    QString runCommand;
    QString workingDirectory;
    QMap<QString, QString> env;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setProjectRoot(const QString& path);

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

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
    void onProjectSearch();
    
    // Status Bar & updates
    void updateStatusBar(const QString& filePath);
    void onActiveFileChanged(const QString& filePath);
    void onCursorPositionChanged(const QString& label);

    // Build & Run
    void onBuildTriggered();
    void onRunTriggered();
    void onStopTriggered();
    
    void onBuildStarted();
    void onBuildOutput(const QString& text, bool isError);
    void onBuildFinished(bool success, int exitCode);
    
    void onRunStarted();
    void onRunOutput(const QString& text, bool isError);
    void onRunFinished(int exitCode);
    
    // Debug Actions
    void onDebugStarted();
    void onDebugEnded();
    void onStepOver();
    void onStepInto();
    void onStepOut();
    void onExpressionEvaluated(const QString& expr);
    
    void onNewProjectFromTemplate();
    void onToggleCommandPalette();
    void onActiveRunConfigChanged(int index);
    
    void handleGitFileSelected(const QString& relPath);
    void handleProblemSelected(const QString& path, int line, int col);
    void onBranchIndicatorClicked();
    void onLanguageIndicatorClicked();

    // Workbench Layout Handlers
    void handleActivityChanged(int index);
    void handleToggleSidebar();

    void handleEditorChanged(CodeEditor* editor);
    void handleEditorTextChanged();
    void handleLspDiagnostics(const QString& uri, const QJsonArray& diagnostics);

private:
    void setupWorkbench();
    void createMenus();
    void setupCommands();
    void createFindPanel();
    
    void loadRunConfigurations();
    void saveRunConfigurations();
    void updateRunConfigurationsDropdown();
    void generateDefaultRunConfig(const QString& filePath);
    
    void loadWorkspaceState();
    void saveWorkspaceState();

    // Managers / Services
    GitManager* m_gitManager;
    BuildManager* m_buildManager;
    RunManager* m_runManager;
    DebugManager* m_debugManager;
    AiAssistant* m_aiAssistant;
    WorkspaceManager* m_workspaceManager;
    ExtensionManager* m_extensionManager;
    LspClient* m_lspClient;

    // Workbench Components
    ActivityBar* m_activityBar;
    Sidebar* m_sidebar;
    EditorArea* m_editorArea;
    BottomPanel* m_bottomPanel;
    AiPanel* m_aiPanel;
    StatusBar* m_statusBar;
    CommandPalette* m_commandPalette;

    // Sidebar View widgets
    ExplorerView* m_explorerView;
    SearchView* m_searchView;
    SourceControlView* m_sourceControlView;
    RunDebugView* m_runDebugView;
    ExtensionsView* m_extensionsView;

    // Splitters
    QSplitter* m_sidebarSplitter;
    QSplitter* m_verticalSplitter;
    
    // Legacy Find/Replace panel
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

    // State
    QString m_projectRoot;
    QVector<RunConfig> m_runConfigs;
    QString m_activeRunConfigName;
    bool m_isProjectLoaded = false;
    CodeEditor* m_currentEditor = nullptr;
};
