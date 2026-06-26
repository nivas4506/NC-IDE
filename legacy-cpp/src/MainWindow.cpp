// src/MainWindow.cpp
#include "MainWindow.h"
#include "settings/SettingsManager.h"
#include "services/ServiceLocator.h"
#include "commands/CommandRegistry.h"
#include "core/FileTypeRegistry.h"
#include "scm/CommitDialog.h"
#include "search/SearchManager.h"
#include <QMenuBar>
#include <QMenu>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QShortcut>
#include <QKeySequence>
#include <QApplication>
#include <QScrollBar>
#include <QTimer>

static QString substituteVariables(QString cmd, const QString& filePath, const QString& workspaceFolder) {
    QFileInfo fileInfo(filePath);
    QString fileDirname = fileInfo.absolutePath();
    QString fileBasename = fileInfo.fileName();
    QString fileBasenameNoExtension = fileInfo.completeBaseName();
    
    cmd.replace("${workspaceFolder}", workspaceFolder.isEmpty() ? QDir::currentPath() : workspaceFolder);
    cmd.replace("${file}", filePath);
    cmd.replace("${fileDirname}", fileDirname);
    cmd.replace("${fileBasename}", fileBasename);
    cmd.replace("${fileBasenameNoExtension}", fileBasenameNoExtension);
    return cmd;
}

static void applyApplicationStyle(const QString& theme) {
    if (theme == "light") {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #f0f0f0; color: #000000; }"
            "QMenuBar { background-color: #e0e0e0; color: #000000; border-bottom: 1px solid #d0d0d0; }"
            "QMenuBar::item:selected { background-color: #d0d0d0; }"
            "QMenu { background-color: #f0f0f0; color: #000000; border: 1px solid #d0d0d0; }"
            "QMenu::item:selected { background-color: #d0d0d0; }"
            "QLineEdit { background-color: #ffffff; color: #000000; border: 1px solid #c0c0c0; border-radius: 3px; padding: 3px; }"
            "QPushButton { background-color: #e0e0e0; color: #000000; border: 1px solid #c0c0c0; border-radius: 3px; padding: 4px 8px; }"
            "QPushButton:hover { background-color: #d0d0d0; }"
            "QLabel { color: #000000; }"
        );
    } else {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #1e1e24; color: #f8f8f2; }"
            "QMenuBar { background-color: #282a36; color: #f8f8f2; border-bottom: 1px solid #44475a; }"
            "QMenuBar::item:selected { background-color: #44475a; }"
            "QMenu { background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; }"
            "QMenu::item:selected { background-color: #44475a; }"
            "QLineEdit { background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; border-radius: 3px; padding: 3px; }"
            "QPushButton { background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; border-radius: 3px; padding: 4px 8px; }"
            "QPushButton:hover { background-color: #6272a4; }"
            "QCheckBox { color: #f8f8f2; }"
            "QLabel { color: #f8f8f2; }"
            "QSplitter::handle { background-color: #282a36; }"
        );
    }
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("NC IDE - Modern Modular Workspace");
    resize(1200, 800);

    // 1. Instantiate Managers/Services
    m_gitManager = new GitManager(this);
    m_buildManager = new BuildManager(this);
    m_runManager = new RunManager(this);
    m_debugManager = new DebugManager(this);
    m_workspaceManager = new WorkspaceManager(this);
    m_extensionManager = new ExtensionManager(this);
    m_lspClient = new LspClient(this);
    m_aiAssistant = new AiAssistant(new NetworkManager(this), this);

    // Register inside ServiceLocator
    ServiceLocator::registerGitManager(m_gitManager);
    ServiceLocator::registerBuildManager(m_buildManager);
    ServiceLocator::registerRunManager(m_runManager);
    ServiceLocator::registerWorkspaceManager(m_workspaceManager);
    ServiceLocator::registerExtensionManager(m_extensionManager);

    // 2. Build Splitter Workbench Layout
    setupWorkbench();

    // 3. Menu and commands registry setup
    createMenus();
    setupCommands();

    // 4. Command Palette init
    m_commandPalette = new CommandPalette(this);
    new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P), this, SLOT(onToggleCommandPalette()));

    // 5. Connect Editor Area Signals
    connect(m_editorArea, &EditorArea::activeFileChanged, this, &MainWindow::onActiveFileChanged);
    connect(m_editorArea, &EditorArea::activeEditorChanged, this, &MainWindow::handleEditorChanged);
    connect(m_lspClient, &LspClient::diagnosticsReceived, this, &MainWindow::handleLspDiagnostics);
    
    // Start clangd language server
    m_lspClient->startServer(Language::Cpp, "clangd", QStringList());
    
    // Theme sync
    SettingsManager& settings = SettingsManager::instance();
    applyApplicationStyle(settings.theme());
    connect(&settings, &SettingsManager::themeChanged, this, [](const QString& theme) {
        applyApplicationStyle(theme);
    });

    // Create a first tab if workspace is empty
    QTimer::singleShot(200, this, [this]() {
        if (m_editorArea->groups().first()->tabWidget()->count() == 0) {
            onNewFile();
        }
    });
}

MainWindow::~MainWindow() {}

void MainWindow::setupWorkbench() {
    QWidget* centralContainer = new QWidget(this);
    setCentralWidget(centralContainer);

    QVBoxLayout* mainVLayout = new QVBoxLayout(centralContainer);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);

    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->setContentsMargins(0, 0, 0, 0);
    topHLayout->setSpacing(0);
    mainVLayout->addLayout(topHLayout, 1);

    // Activity Bar
    m_activityBar = new ActivityBar(this);
    topHLayout->addWidget(m_activityBar);

    // Sidebar Splitter (Sidebar + vertical editor splits)
    m_sidebarSplitter = new QSplitter(Qt::Horizontal, this);
    m_sidebarSplitter->setChildrenCollapsible(false);
    topHLayout->addWidget(m_sidebarSplitter, 1);

    // Sidebar stacked view
    m_sidebar = new Sidebar(this);
    m_sidebarSplitter->addWidget(m_sidebar);

    // Vertical Splitter (EditorContainer + BottomPanel)
    m_verticalSplitter = new QSplitter(Qt::Vertical, this);
    m_verticalSplitter->setChildrenCollapsible(false);
    m_sidebarSplitter->addWidget(m_verticalSplitter);

    // Editor Area Container (splits + inline find panel)
    QWidget* edContainer = new QWidget(this);
    QVBoxLayout* edLayout = new QVBoxLayout(edContainer);
    edLayout->setContentsMargins(0, 0, 0, 0);
    edLayout->setSpacing(0);

    m_editorArea = new EditorArea(this);
    edLayout->addWidget(m_editorArea, 1);

    createFindPanel();
    edLayout->addWidget(m_findPanel);
    m_verticalSplitter->addWidget(edContainer);

    // Bottom Console Panel
    m_bottomPanel = new BottomPanel(this);
    m_verticalSplitter->addWidget(m_bottomPanel);

    // Set initial size ratios
    m_sidebarSplitter->setSizes({260, 940});
    m_verticalSplitter->setSizes({550, 250});

    // Instantiate Sidebar Views
    m_explorerView = new ExplorerView(this);
    m_searchView = new SearchView(this);
    m_sourceControlView = new SourceControlView(m_gitManager, this);
    m_runDebugView = new RunDebugView(this);
    m_extensionsView = new ExtensionsView(m_extensionManager, this);
    m_aiPanel = new AiPanel(m_aiAssistant, this);

    m_sidebar->addView(m_explorerView, "Explorer");
    m_sidebar->addView(m_searchView, "Search");
    m_sidebar->addView(m_sourceControlView, "Source Control");
    m_sidebar->addView(m_runDebugView, "Run & Debug");
    m_sidebar->addView(m_extensionsView, "Extensions");
    m_sidebar->addView(m_aiPanel, "AI Assistant");

    // Status Bar
    m_statusBar = new StatusBar(this);
    setStatusBar(nullptr); // Remove default status bar
    mainVLayout->addWidget(m_statusBar);

    // Workbench Signals
    connect(m_activityBar, &ActivityBar::activityChanged, this, &MainWindow::handleActivityChanged);
    connect(m_activityBar, &ActivityBar::toggleSidebar, this, &MainWindow::handleToggleSidebar);
    connect(m_sidebar, &Sidebar::collapseRequested, this, &MainWindow::handleToggleSidebar);

    // Connect View Selectors
    connect(m_explorerView, &ExplorerView::fileSelected, m_editorArea, &EditorArea::openFile);
    connect(m_searchView, &SearchView::matchSelected, this, &MainWindow::handleProblemSelected);
    connect(m_sourceControlView, &SourceControlView::fileSelected, this, &MainWindow::handleGitFileSelected);

    connect(m_statusBar, &StatusBar::branchIndicatorClicked, this, &MainWindow::onBranchIndicatorClicked);
    connect(m_statusBar, &StatusBar::languageIndicatorClicked, this, &MainWindow::onLanguageIndicatorClicked);

    // Run & Debug panel triggers
    connect(m_runDebugView, &RunDebugView::buildTriggered, this, &MainWindow::onBuildTriggered);
    connect(m_runDebugView, &RunDebugView::runTriggered, this, &MainWindow::onRunTriggered);
    connect(m_runDebugView, &RunDebugView::stopTriggered, this, &MainWindow::onStopTriggered);
    connect(m_runDebugView, &RunDebugView::activeConfigurationChanged, this, &MainWindow::onActiveRunConfigChanged);
    connect(m_runDebugView, &RunDebugView::editConfigurationRequested, this, [this]() {
        if (!m_projectRoot.isEmpty()) {
            m_editorArea->openFile(QDir(m_projectRoot).absoluteFilePath(".ncide/runconfig.json"));
        }
    });
    connect(m_runDebugView, &RunDebugView::stepOverTriggered, this, &MainWindow::onStepOver);
    connect(m_runDebugView, &RunDebugView::stepIntoTriggered, this, &MainWindow::onStepInto);
    connect(m_runDebugView, &RunDebugView::stepOutTriggered, this, &MainWindow::onStepOut);

    // Wire Build Output Panel
    connect(m_buildManager, &BuildManager::buildStarted, this, &MainWindow::onBuildStarted);
    connect(m_buildManager, &BuildManager::buildOutput, this, &MainWindow::onBuildOutput);
    connect(m_buildManager, &BuildManager::buildFinished, this, &MainWindow::onBuildFinished);
    
    // Wire Runner Panel
    connect(m_runManager, &RunManager::runStarted, this, &MainWindow::onRunStarted);
    connect(m_runManager, &RunManager::runOutput, this, &MainWindow::onRunOutput);
    connect(m_runManager, &RunManager::runFinished, this, &MainWindow::onRunFinished);
}

void MainWindow::handleActivityChanged(int index) {
    if (m_sidebar->isHidden()) {
        m_sidebar->show();
        m_sidebarSplitter->setSizes({260, 940});
    }
    m_sidebar->showView(index);
}

void MainWindow::handleToggleSidebar() {
    if (m_sidebar->isHidden()) {
        m_sidebar->show();
        m_sidebarSplitter->setSizes({260, 940});
    } else {
        m_sidebar->hide();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    saveWorkspaceState();
    m_editorArea->closeAll();
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (m_commandPalette && m_commandPalette->isVisible()) {
        int x = (width() - m_commandPalette->width()) / 2;
        m_commandPalette->move(x, 50);
    }
}

void MainWindow::setProjectRoot(const QString& path) {
    m_projectRoot = path;
    m_isProjectLoaded = true;

    m_explorerView->clearWorkspace();
    m_explorerView->addRootFolder(path);

    m_searchView->setProjectRoot(path);
    m_gitManager->setRootPath(path);
    m_workspaceManager->setProjectRoot(path);
    m_bottomPanel->terminalPanel()->setWorkingDirectory(path);

    SettingsManager::instance().addRecentProject(path);
    m_statusBar->showNotification("Workspace root set to: " + QFileInfo(path).fileName());
    setWindowTitle("NC IDE - " + QFileInfo(path).fileName());

    m_editorArea->setProjectRoot(path);

    loadRunConfigurations();
    loadWorkspaceState();
}

void MainWindow::onNewFile() {
    m_editorArea->newUntitledTab();
}

void MainWindow::onOpenFile() {
    QString path = QFileDialog::getOpenFileName(this, "Open File");
    if (!path.isEmpty()) {
        m_editorArea->openFile(path);
    }
}

void MainWindow::onOpenProject() {
    QString path = QFileDialog::getExistingDirectory(this, "Open Project/Folder");
    if (!path.isEmpty()) {
        setProjectRoot(path);
    }
}

void MainWindow::onSave() {
    m_editorArea->saveCurrentFile();
    // Re-detect run config on first save of a tab
    QString path = m_editorArea->currentFilePath();
    if (m_runConfigs.isEmpty() && !path.isEmpty()) {
        generateDefaultRunConfig(path);
    }
}

void MainWindow::onSaveAs() {
    int index = m_editorArea->activeGroup()->tabWidget()->currentIndex();
    if (index >= 0) {
        m_editorArea->activeGroup()->tabManager()->saveTabAs(index, QString());
    }
}

void MainWindow::onSaveAll() {
    m_editorArea->saveAll();
}

void MainWindow::onCloseTab() {
    m_editorArea->closeCurrentTab();
}

void MainWindow::onCloseAll() {
    m_editorArea->closeAll();
}

void MainWindow::onRecentFileTriggered() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        m_editorArea->openFile(action->data().toString());
    }
}

void MainWindow::onUndo() {
    CodeEditor* ed = m_editorArea->currentEditor();
    if (ed) ed->undo();
}

void MainWindow::onRedo() {
    CodeEditor* ed = m_editorArea->currentEditor();
    if (ed) ed->redo();
}

void MainWindow::onGoToLine() {
    CodeEditor* ed = m_editorArea->currentEditor();
    if (!ed) return;
    bool ok;
    int line = QInputDialog::getInt(this, "Go to Line", "Line number:",
                                    ed->textCursor().blockNumber() + 1, 1, ed->blockCount(), 1, &ok);
    if (ok) {
        ed->goToLine(line);
    }
}

void MainWindow::onShowPreferences() {
    SettingsManager& settings = SettingsManager::instance();
    QDialog dialog(this);
    dialog.setWindowTitle("Preferences");
    dialog.setMinimumWidth(300);

    QFormLayout form(&dialog);

    QComboBox themeCombo;
    themeCombo.addItem("dark");
    themeCombo.addItem("light");
    themeCombo.setCurrentText(settings.theme());

    QComboBox fontCombo;
    fontCombo.addItems({"Consolas", "Courier New", "JetBrains Mono", "Monospace"});
    fontCombo.setCurrentText(settings.fontFamily());

    QSpinBox sizeSpin;
    sizeSpin.setRange(8, 30);
    sizeSpin.setValue(settings.fontSize());

    form.addRow("Theme:", &themeCombo);
    form.addRow("Editor Font:", &fontCombo);
    form.addRow("Font Size:", &sizeSpin);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        settings.setTheme(themeCombo.currentText());
        settings.setFontFamily(fontCombo.currentText());
        settings.setFontSize(sizeSpin.value());
    }
}

void MainWindow::onToggleFindPanel(bool replaceMode) {
    if (m_findPanel->isVisible() && m_replaceRowWidget->isVisible() == replaceMode) {
        m_findPanel->hide();
    } else {
        m_findPanel->show();
        m_replaceRowWidget->setVisible(replaceMode);
        m_findInput->setFocus();
        m_findInput->selectAll();
        
        CodeEditor* editor = m_editorArea->currentEditor();
        if (editor) {
            QString selection = editor->textCursor().selectedText();
            if (!selection.isEmpty() && !selection.contains(QChar::ParagraphSeparator)) {
                m_findInput->setText(selection);
            }
        }
    }
}

void MainWindow::onProjectSearch() {
    m_activityBar->setActiveIndex(1); // open search view
    m_sidebar->showView(1);
    if (m_sidebar->isHidden()) {
        m_sidebar->show();
    }
}

void MainWindow::updateStatusBar(const QString& filePath) {
    if (filePath.isEmpty()) {
        m_statusBar->setLanguage("PlainText");
        m_statusBar->setCursorPos("Ln 1, Col 1");
    } else {
        CodeEditor* editor = m_editorArea->currentEditor();
        if (editor) {
            FileTypeInfo info = FileTypeRegistry::detect(filePath);
            m_statusBar->setLanguage(info.languageName);
            m_statusBar->setCursorPos(editor->cursorPositionLabel());
        }
    }
}

void MainWindow::onActiveFileChanged(const QString& filePath) {
    updateStatusBar(filePath);
    CodeEditor* editor = m_editorArea->currentEditor();
    if (editor) {
        disconnect(editor, &CodeEditor::cursorPositionChanged2, this, &MainWindow::onCursorPositionChanged);
        connect(editor, &CodeEditor::cursorPositionChanged2, this, &MainWindow::onCursorPositionChanged);
    }
    
    if (m_runConfigs.isEmpty() && !filePath.isEmpty()) {
        generateDefaultRunConfig(filePath);
    }
}

void MainWindow::onCursorPositionChanged(const QString& label) {
    m_statusBar->setCursorPos(label);
}

void MainWindow::onBuildTriggered() {
    m_bottomPanel->problemsPanel()->clearDiagnostics();
    m_bottomPanel->outputView()->clear();

    QString activeConfig = m_runDebugView->activeConfiguration();
    int idx = -1;
    for (int i = 0; i < m_runConfigs.size(); ++i) {
        if (m_runConfigs[i].name == activeConfig) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        m_bottomPanel->outputView()->appendPlainText("No launch configuration selected.");
        return;
    }

    RunConfig active = m_runConfigs[idx];
    if (active.buildCommand.isEmpty()) {
        m_bottomPanel->outputView()->appendPlainText("No build command required.");
        onBuildFinished(true, 0);
        return;
    }

    QString subCmd = substituteVariables(active.buildCommand, m_editorArea->currentFilePath(), m_projectRoot);
    QString subDir = substituteVariables(active.workingDirectory, m_editorArea->currentFilePath(), m_projectRoot);

    m_bottomPanel->outputView()->appendPlainText("Building: " + subCmd + "\n");
    m_bottomPanel->showOutput();
    m_buildManager->build(subCmd, subDir);
}

void MainWindow::onRunTriggered() {
    QString activeConfig = m_runDebugView->activeConfiguration();
    int idx = -1;
    for (int i = 0; i < m_runConfigs.size(); ++i) {
        if (m_runConfigs[i].name == activeConfig) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        m_statusBar->showNotification("No configuration loaded.");
        return;
    }

    RunConfig active = m_runConfigs[idx];
    if (!active.buildCommand.isEmpty()) {
        QMetaObject::Connection* conn = new QMetaObject::Connection();
        *conn = connect(m_buildManager, &BuildManager::buildFinished, this, [this, conn, active](bool success, int) {
            disconnect(*conn);
            delete conn;

            if (success) {
                QString subCmd = substituteVariables(active.runCommand, m_editorArea->currentFilePath(), m_projectRoot);
                QString subDir = substituteVariables(active.workingDirectory, m_editorArea->currentFilePath(), m_projectRoot);
                m_bottomPanel->outputView()->appendPlainText("\nRunning: " + subCmd + "\n");
                m_runManager->run(subCmd, subDir, active.env);
            }
        });
        onBuildTriggered();
    } else {
        QString subCmd = substituteVariables(active.runCommand, m_editorArea->currentFilePath(), m_projectRoot);
        QString subDir = substituteVariables(active.workingDirectory, m_editorArea->currentFilePath(), m_projectRoot);
        m_bottomPanel->problemsPanel()->clearDiagnostics();
        m_bottomPanel->outputView()->clear();
        m_bottomPanel->outputView()->appendPlainText("Running: " + subCmd + "\n");
        m_bottomPanel->showOutput();
        m_runManager->run(subCmd, subDir, active.env);
    }
}

void MainWindow::onStopTriggered() {
    m_runManager->stop();
    m_buildManager->cancelBuild();
}

void MainWindow::onBuildStarted() {
    m_statusBar->showNotification("Compilation started...");
}

void MainWindow::onBuildOutput(const QString& text, bool isError) {
    if (isError) {
        m_bottomPanel->outputView()->appendHtml("<span style='color:#ff5555; white-space: pre-wrap; font-family: monospace;'>" + text.toHtmlEscaped() + "</span>");
    } else {
        m_bottomPanel->outputView()->appendPlainText(text);
    }

    QVector<Diagnostic> diags = ProblemParser::parseBuildOutput(text);
    if (!diags.isEmpty()) {
        m_bottomPanel->problemsPanel()->addDiagnostics(diags);
    }
}

void MainWindow::onBuildFinished(bool success, int exitCode) {
    if (success) {
        m_bottomPanel->outputView()->appendPlainText(QString("\nBuild finished successfully with code %1.").arg(exitCode));
    } else {
        m_bottomPanel->outputView()->appendHtml(QString("<br/><span style='color:#ff5555; font-weight: bold;'>Build failed with code %1.</span>").arg(exitCode));
        m_bottomPanel->showProblems();
    }
}

void MainWindow::onRunStarted() {
    m_statusBar->showNotification("Execution started...");
}

void MainWindow::onRunOutput(const QString& text, bool isError) {
    if (isError) {
        m_bottomPanel->outputView()->appendHtml("<span style='color:#ff5555; white-space: pre-wrap; font-family: monospace;'>" + text.toHtmlEscaped() + "</span>");
    } else {
        m_bottomPanel->outputView()->appendPlainText(text);
    }

    QVector<Diagnostic> diags = ProblemParser::parseRuntimeOutput(text);
    if (!diags.isEmpty()) {
        m_bottomPanel->problemsPanel()->addDiagnostics(diags);
        m_bottomPanel->showProblems();
    }
}

void MainWindow::onRunFinished(int exitCode) {
    m_bottomPanel->outputView()->appendPlainText(QString("\nProcess exited with code %1.").arg(exitCode));
    m_statusBar->showNotification(QString("Process exited with code %1").arg(exitCode));
}

void MainWindow::onDebugStarted() {
    m_statusBar->showNotification("Debug session started.");
    m_bottomPanel->showDebugConsole();
}

void MainWindow::onDebugEnded() {
    m_statusBar->showNotification("Debug session ended.");
}

void MainWindow::onStepOver() {
    m_debugManager->stepOver();
}

void MainWindow::onStepInto() {
    m_debugManager->stepInto();
}

void MainWindow::onStepOut() {
    m_debugManager->stepOut();
}

void MainWindow::onExpressionEvaluated(const QString& expr) {
    m_debugManager->evaluateExpression(expr);
}

void MainWindow::handleGitFileSelected(const QString& relPath) {
    if (m_projectRoot.isEmpty()) return;
    QString absPath = QDir(m_projectRoot).absoluteFilePath(relPath);

    QProcess* diffProc = new QProcess(this);
    diffProc->setWorkingDirectory(m_projectRoot);
    connect(diffProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, diffProc, relPath](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && exitCode == 0) {
            QString diffText = QString::fromLocal8Bit(diffProc->readAllStandardOutput());
            if (diffText.isEmpty()) {
                diffText = "No unstaged changes.";
            }
            
            CodeEditor* editor = m_editorArea->newUntitledTab();
            editor->setLanguage(Language::PlainText);
            editor->setPlainText(diffText);
            editor->setReadOnly(true);
            
            int idx = m_editorArea->activeGroup()->tabWidget()->currentIndex();
            m_editorArea->activeGroup()->tabWidget()->setTabText(idx, "Diff: " + QFileInfo(relPath).fileName());
        }
        diffProc->deleteLater();
    });
    
    diffProc->start("git", QStringList() << "diff" << "--" << relPath);
}

void MainWindow::handleProblemSelected(const QString& path, int line, int col) {
    QString fullPath = path;
    if (QFileInfo(path).isRelative() && !m_projectRoot.isEmpty()) {
        fullPath = QDir(m_projectRoot).absoluteFilePath(path);
    }
    
    CodeEditor* editor = m_editorArea->openFile(fullPath);
    if (editor) {
        editor->goToLine(line);
        if (col > 0) {
            QTextCursor cursor = editor->textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, col - 1);
            editor->setTextCursor(cursor);
        }
        editor->setFocus();
    }
}

void MainWindow::onBranchIndicatorClicked() {
    if (m_projectRoot.isEmpty()) return;
    m_gitManager->refreshBranches();
    
    QMetaObject::Connection* conn = new QMetaObject::Connection();
    *conn = connect(m_gitManager, &GitManager::branchesChanged, this, [this, conn](const QStringList& branches, const QString& currentBranch) {
        disconnect(*conn);
        delete conn;
        
        if (branches.isEmpty()) return;
        
        bool ok;
        QString item = QInputDialog::getItem(this, "Git Checkout", "Select branch to checkout:", branches, branches.indexOf(currentBranch), false, &ok);
        if (ok && !item.isEmpty() && item != currentBranch) {
            m_gitManager->checkoutBranch(item);
            m_statusBar->setGitBranch(item);
        }
    });
}

void MainWindow::onLanguageIndicatorClicked() {
    CodeEditor* editor = m_editorArea->currentEditor();
    if (!editor) return;

    QStringList languages = {"PlainText", "C", "C++", "Python", "Java", "JavaScript", "HTML", "CSS", "JSON", "Markdown"};
    bool ok;
    QString selection = QInputDialog::getItem(this, "Select Language Mode", "Language:", languages, 0, false, &ok);
    if (ok && !selection.isEmpty()) {
        Language lang = Language::PlainText;
        if (selection == "C" || selection == "C++") lang = Language::Cpp;
        else if (selection == "Python") lang = Language::Python;
        else if (selection == "Java") lang = Language::Java;
        else if (selection == "JavaScript" || selection == "JSON") lang = Language::JavaScript;
        else if (selection == "HTML" || selection == "CSS" || selection == "Markdown") lang = Language::Html;
        
        editor->setLanguage(lang);
        m_statusBar->setLanguage(selection);
    }
}

void MainWindow::onToggleCommandPalette() {
    if (m_commandPalette->isVisible()) {
        m_commandPalette->hidePalette();
    } else {
        m_commandPalette->showPalette();
    }
}

void MainWindow::onActiveRunConfigChanged(int index) {
    if (index >= 0 && index < m_runConfigs.size()) {
        m_activeRunConfigName = m_runConfigs[index].name;
        saveRunConfigurations();
    }
}

// Workspace Loading
void MainWindow::loadRunConfigurations() {
    m_runConfigs.clear();
    if (m_projectRoot.isEmpty()) return;

    QString configPath = QDir(m_projectRoot).absoluteFilePath(".ncide/runconfig.json");
    if (!QFile::exists(configPath)) {
        QString activeFile = m_editorArea->currentFilePath();
        if (!activeFile.isEmpty()) {
            generateDefaultRunConfig(activeFile);
        }
        return;
    }

    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject rootObj = doc.object();
        QJsonArray configsArr = rootObj.value("configurations").toArray();
        for (int i = 0; i < configsArr.size(); ++i) {
            QJsonObject confObj = configsArr[i].toObject();
            RunConfig config;
            config.name = confObj.value("name").toString();
            config.language = confObj.value("language").toString();
            config.buildCommand = confObj.value("buildCommand").toString();
            config.runCommand = confObj.value("runCommand").toString();
            config.workingDirectory = confObj.value("workingDirectory").toString("${workspaceFolder}");
            
            QJsonObject envObj = confObj.value("env").toObject();
            for (const QString& k : envObj.keys()) {
                config.env[k] = envObj.value(k).toString();
            }
            m_runConfigs.append(config);
        }
        m_activeRunConfigName = rootObj.value("activeConfiguration").toString();
    }

    updateRunConfigurationsDropdown();
}

void MainWindow::saveRunConfigurations() {
    if (m_projectRoot.isEmpty()) return;

    QDir projectDir(m_projectRoot);
    if (!projectDir.exists(".ncide")) {
        projectDir.mkdir(".ncide");
    }

    QFile file(projectDir.absoluteFilePath(".ncide/runconfig.json"));
    if (!file.open(QIODevice::WriteOnly)) return;

    QJsonObject rootObj;
    QJsonArray configsArr;
    for (const RunConfig& conf : m_runConfigs) {
        QJsonObject confObj;
        confObj["name"] = conf.name;
        confObj["language"] = conf.language;
        confObj["buildCommand"] = conf.buildCommand;
        confObj["runCommand"] = conf.runCommand;
        confObj["workingDirectory"] = conf.workingDirectory;
        
        QJsonObject envObj;
        for (auto it = conf.env.begin(); it != conf.env.end(); ++it) {
            envObj[it.key()] = it.value();
        }
        confObj["env"] = envObj;
        configsArr.append(confObj);
    }
    rootObj["configurations"] = configsArr;
    rootObj["activeConfiguration"] = m_activeRunConfigName;

    QJsonDocument doc(rootObj);
    file.write(doc.toJson());
    file.close();
}

void MainWindow::updateRunConfigurationsDropdown() {
    QStringList names;
    for (const RunConfig& conf : m_runConfigs) {
        names.append(conf.name);
    }
    m_runDebugView->setConfigurations(names, m_activeRunConfigName);
}

void MainWindow::handleEditorChanged(CodeEditor* editor) {
    if (m_currentEditor) {
        disconnect(m_currentEditor, &CodeEditor::textChanged, this, &MainWindow::handleEditorTextChanged);
    }
    m_currentEditor = editor;
    if (m_currentEditor) {
        connect(m_currentEditor, &CodeEditor::textChanged, this, &MainWindow::handleEditorTextChanged);
        
        // Notify LSP about file open
        QString filePath = m_editorArea->currentFilePath();
        if (!filePath.isEmpty()) {
            m_lspClient->didOpen("file://" + filePath, m_currentEditor->toPlainText());
        }
    }
}

void MainWindow::handleEditorTextChanged() {
    if (m_currentEditor) {
        QString filePath = m_editorArea->currentFilePath();
        if (!filePath.isEmpty()) {
            m_lspClient->didChange("file://" + filePath, m_currentEditor->toPlainText());
        }
    }
}

void MainWindow::handleLspDiagnostics(const QString& uri, const QJsonArray& diagnostics) {
    QVector<Diagnostic> diags;
    for (int i = 0; i < diagnostics.size(); ++i) {
        QJsonObject d = diagnostics[i].toObject();
        Diagnostic diag;
        
        QString file = uri;
        if (file.startsWith("file://")) file = file.mid(7);
        
        diag.filePath = file;
        diag.line = d["range"].toObject()["start"].toObject()["line"].toInt() + 1;
        diag.column = d["range"].toObject()["start"].toObject()["character"].toInt() + 1;
        diag.message = d["message"].toString();
        
        int severity = d["severity"].toInt();
        diag.severity = (severity == 1) ? "Error" : "Warning";
        diag.source = "LSP";
        
        diags.append(diag);
    }
    
    m_bottomPanel->problemsPanel()->clearDiagnostics();
    if (!diags.isEmpty()) {
        m_bottomPanel->problemsPanel()->addDiagnostics(diags);
    }
}

void MainWindow::generateDefaultRunConfig(const QString& filePath) {
    FileTypeInfo info = FileTypeRegistry::detect(filePath);
    if (info.language == Language::PlainText || info.runCommand.isEmpty()) return;

    RunConfig config;
    config.name = "Run " + QFileInfo(filePath).fileName();
    config.language = info.languageName.toLower();
    config.buildCommand = info.buildCommand;
    config.runCommand = info.runCommand;
    config.workingDirectory = "${workspaceFolder}";

    m_runConfigs.clear();
    m_runConfigs.append(config);
    m_activeRunConfigName = config.name;

    updateRunConfigurationsDropdown();
    saveRunConfigurations();
}

void MainWindow::loadWorkspaceState() {
    WorkspaceState ws;
    if (!m_workspaceManager->loadWorkspace(ws)) return;

    m_editorArea->closeAll();

    // Reopen saved tabs inside active group
    for (const TabState& ts : ws.openTabs) {
        CodeEditor* ed = m_editorArea->openFile(ts.path);
        if (ed) {
            ed->goToLine(ts.cursorLine);
            QTextCursor tc = ed->textCursor();
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, ts.cursorCol - 1);
            ed->setTextCursor(tc);
        }
    }

    if (!ws.terminal.cwd.isEmpty()) {
        m_bottomPanel->terminalPanel()->setWorkingDirectory(ws.terminal.cwd);
    }
    m_bottomPanel->terminalPanel()->setHistory(ws.terminal.history);

    if (!ws.activeRunConfiguration.isEmpty()) {
        m_activeRunConfigName = ws.activeRunConfiguration;
        updateRunConfigurationsDropdown();
    }
}

void MainWindow::saveWorkspaceState() {
    if (m_projectRoot.isEmpty()) return;

    WorkspaceState ws;
    ws.rootPath = m_projectRoot;

    // Loop through all active EditorGroups
    for (EditorGroup* group : m_editorArea->groups()) {
        for (int i = 0; i < group->tabWidget()->count(); ++i) {
            CodeEditor* ed = group->tabManager()->editorAt(i);
            if (!ed) continue;

            TabState ts;
            ts.path = group->tabManager()->filePathAt(i);
            if (ts.path.isEmpty()) continue;

            ts.cursorLine = ed->textCursor().blockNumber() + 1;
            ts.cursorCol = ed->textCursor().columnNumber() + 1;
            ts.scrollPosition = ed->verticalScrollBar()->value();
            ws.openTabs.append(ts);
        }
    }

    ws.activeTab = m_editorArea->currentFilePath();
    ws.terminal.cwd = m_bottomPanel->terminalPanel()->workingDirectory();
    ws.terminal.history = m_bottomPanel->terminalPanel()->history();
    ws.activeRunConfiguration = m_activeRunConfigName;

    m_workspaceManager->saveWorkspace(ws);
}

void MainWindow::createFindPanel() {
    m_findPanel = new QWidget(this);
    m_findPanel->setObjectName("FindPanel");
    m_findPanel->hide();

    QVBoxLayout* mainLayout = new QVBoxLayout(m_findPanel);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);

    QWidget* findRowWidget = new QWidget(m_findPanel);
    QHBoxLayout* findLayout = new QHBoxLayout(findRowWidget);
    findLayout->setContentsMargins(0, 0, 0, 0);
    findLayout->setSpacing(6);

    findLayout->addWidget(new QLabel("Find:", findRowWidget));
    m_findInput = new QLineEdit(findRowWidget);
    findLayout->addWidget(m_findInput, 1);

    m_findNextBtn = new QPushButton("Next", findRowWidget);
    m_findPrevBtn = new QPushButton("Prev", findRowWidget);
    findLayout->addWidget(m_findNextBtn);
    findLayout->addWidget(m_findPrevBtn);

    m_matchCaseCheck = new QCheckBox("Match Case", findRowWidget);
    m_wholeWordCheck = new QCheckBox("Whole Word", findRowWidget);
    findLayout->addWidget(m_matchCaseCheck);
    findLayout->addWidget(m_wholeWordCheck);

    QPushButton* closeBtn = new QPushButton("X", findRowWidget);
    closeBtn->setFixedWidth(24);
    findLayout->addWidget(closeBtn);

    m_replaceRowWidget = new QWidget(m_findPanel);
    QHBoxLayout* replaceLayout = new QHBoxLayout(m_replaceRowWidget);
    replaceLayout->setContentsMargins(0, 0, 0, 0);
    replaceLayout->setSpacing(6);

    replaceLayout->addWidget(new QLabel("Replace:", m_replaceRowWidget));
    m_replaceInput = new QLineEdit(m_replaceRowWidget);
    replaceLayout->addWidget(m_replaceInput, 1);

    m_replaceBtn = new QPushButton("Replace", m_replaceRowWidget);
    m_replaceAllBtn = new QPushButton("Replace All", m_replaceRowWidget);
    replaceLayout->addWidget(m_replaceBtn);
    replaceLayout->addWidget(m_replaceAllBtn);
    
    replaceLayout->addSpacing(250);

    mainLayout->addWidget(findRowWidget);
    mainLayout->addWidget(m_replaceRowWidget);

    connect(m_findInput, &QLineEdit::returnPressed, this, [this]() {
        SearchOptions opts;
        opts.matchCase = m_matchCaseCheck->isChecked();
        opts.wholeWord = m_wholeWordCheck->isChecked();
        
        // Use active group tab manager search
        CodeEditor* ed = m_editorArea->currentEditor();
        if (ed) {
            // Find and highlight
            ed->find(m_findInput->text());
        }
    });
    connect(m_findNextBtn, &QPushButton::clicked, this, [this]() {
        CodeEditor* ed = m_editorArea->currentEditor();
        if (ed) ed->find(m_findInput->text());
    });
    connect(closeBtn, &QPushButton::clicked, m_findPanel, &QWidget::hide);
}

void MainWindow::createMenus() {
    QMenuBar* bar = menuBar();

    QMenu* fileMenu = bar->addMenu("&File");
    fileMenu->addAction("&New File", QKeySequence::New, this, &MainWindow::onNewFile);
    fileMenu->addAction("&Open File...", QKeySequence::Open, this, &MainWindow::onOpenFile);
    fileMenu->addAction("Open Folder/&Project...", this, &MainWindow::onOpenProject);
    fileMenu->addAction("New Project from &Template...", QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_N), this, &MainWindow::onNewProjectFromTemplate);
    
    fileMenu->addSeparator();
    fileMenu->addAction("&Save", QKeySequence::Save, this, &MainWindow::onSave);
    fileMenu->addAction("Save &As...", QKeySequence::SaveAs, this, &MainWindow::onSaveAs);
    fileMenu->addAction("Save Al&l", QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S), this, &MainWindow::onSaveAll);
    
    fileMenu->addSeparator();
    fileMenu->addAction("&Close Tab", QKeySequence::Close, this, &MainWindow::onCloseTab);
    fileMenu->addAction("Close &All Tabs", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W), this, &MainWindow::onCloseAll);
    
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &MainWindow::close);

    QMenu* editMenu = bar->addMenu("&Edit");
    editMenu->addAction("&Undo", QKeySequence::Undo, this, &MainWindow::onUndo);
    editMenu->addAction("&Redo", QKeySequence::Redo, this, &MainWindow::onRedo);
    editMenu->addSeparator();
    editMenu->addAction("&Preferences...", this, &MainWindow::onShowPreferences);

    QMenu* searchMenu = bar->addMenu("&Search");
    searchMenu->addAction("&Find...", QKeySequence::Find, this, [this]() { onToggleFindPanel(false); });
    searchMenu->addAction("Find & Replace...", QKeySequence(Qt::CTRL | Qt::Key_H), this, [this]() { onToggleFindPanel(true); });
    searchMenu->addAction("Find in &Files (Project)...", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this, &MainWindow::onProjectSearch);
    searchMenu->addAction("&Go to Line...", QKeySequence(Qt::CTRL | Qt::Key_G), this, &MainWindow::onGoToLine);

    QMenu* viewMenu = bar->addMenu("&View");
    viewMenu->addAction("Toggle Split Editor Vertical", this, [this]() { m_editorArea->split(Qt::Horizontal); });
    viewMenu->addAction("Toggle Split Editor Horizontal", this, [this]() { m_editorArea->split(Qt::Vertical); });
    viewMenu->addAction("Command &Palette...", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P), this, &MainWindow::onToggleCommandPalette);
}

void MainWindow::setupCommands() {
    CommandRegistry& reg = CommandRegistry::instance();
    
    reg.registerCommand("file.new", "New File", "File", "Ctrl+N", [this]() { onNewFile(); });
    reg.registerCommand("file.open", "Open File", "File", "Ctrl+O", [this]() { onOpenFile(); });
    reg.registerCommand("project.open", "Open Folder / Project", "Project", "", [this]() { onOpenProject(); });
    reg.registerCommand("project.newTemplate", "New Project from Template", "Project", "Ctrl+Alt+N", [this]() { onNewProjectFromTemplate(); });
    reg.registerCommand("file.save", "Save Current Tab", "File", "Ctrl+S", [this]() { onSave(); });
    reg.registerCommand("file.saveAs", "Save As", "File", "Ctrl+Shift+S", [this]() { onSaveAs(); });
    reg.registerCommand("file.saveAll", "Save All", "File", "Ctrl+Alt+S", [this]() { onSaveAll(); });
    reg.registerCommand("project.build", "Build Project", "Build", "Ctrl+B", [this]() { onBuildTriggered(); });
    reg.registerCommand("project.run", "Run Project", "Run", "F5", [this]() { onRunTriggered(); });
    reg.registerCommand("project.stop", "Stop Execution", "Run", "", [this]() { onStopTriggered(); });
    
    reg.registerCommand("view.terminal", "Toggle Terminal Tab", "View", "", [this]() {
        m_bottomPanel->showTerminal();
    });
    reg.registerCommand("view.problems", "Toggle Problems Panel", "View", "", [this]() {
        m_bottomPanel->showProblems();
    });
    reg.registerCommand("view.build", "Toggle Build Output Panel", "View", "", [this]() {
        m_bottomPanel->showOutput();
    });
    reg.registerCommand("view.splitVertical", "Split Editor Vertically", "View", "", [this]() {
        m_editorArea->split(Qt::Horizontal);
    });
    reg.registerCommand("view.splitHorizontal", "Split Editor Horizontally", "View", "", [this]() {
        m_editorArea->split(Qt::Vertical);
    });
    
    reg.registerCommand("git.commit", "Git: Commit", "Git", "", [this]() {
        CommitDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            m_gitManager->commitChanges(dlg.commitMessage());
        }
    });
    reg.registerCommand("git.pull", "Git: Pull", "Git", "", [this]() { m_gitManager->pull(); });
    reg.registerCommand("git.push", "Git: Push", "Git", "", [this]() { m_gitManager->push(); });
    reg.registerCommand("git.switchBranch", "Git: Switch Branch", "Git", "", [this]() { onBranchIndicatorClicked(); });
}

// Template copier helper
static bool copyDirectoryRecursively(const QString& sourceFolder, const QString& destFolder, const QString& projectName) {
    QDir sourceDir(sourceFolder);
    if (!sourceDir.exists()) return false;
    
    QDir destDir(destFolder);
    if (!destDir.exists()) {
        QDir().mkpath(destFolder);
    }
    
    QStringList files = sourceDir.entryList(QDir::Files);
    for (const QString& file : files) {
        QString srcFilePath = sourceDir.absoluteFilePath(file);
        QString destFilePath = destDir.absoluteFilePath(file);
        
        QFile srcFile(srcFilePath);
        if (srcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = QString::fromUtf8(srcFile.readAll());
            srcFile.close();
            
            content.replace("{{PROJECT_NAME}}", projectName);
            content.replace("TempProject", projectName);
            
            QFile outFile(destFilePath);
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream outStream(&outFile);
                outStream << content;
                outFile.close();
            }
        }
    }
    
    QStringList dirs = sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& dir : dirs) {
        QString srcDirPath = sourceDir.absoluteFilePath(dir);
        QString destDirPath = destDir.absoluteFilePath(dir);
        copyDirectoryRecursively(srcDirPath, destDirPath, projectName);
    }
    return true;
}

void MainWindow::onNewProjectFromTemplate() {
    QStringList templates = {"cpp", "python", "java", "node", "web"};
    bool ok;
    QString templateType = QInputDialog::getItem(this, "New Project Template", "Select project template:", templates, 0, false, &ok);
    if (!ok || templateType.isEmpty()) return;
    
    QString destDir = QFileDialog::getExistingDirectory(this, "Select Project Parent Directory");
    if (destDir.isEmpty()) return;
    
    QString projectName = QInputDialog::getText(this, "Project Name", "Enter project name:", QLineEdit::Normal, "MyProject", &ok);
    if (!ok || projectName.isEmpty()) return;
    
    QString finalProjectPath = QDir(destDir).absoluteFilePath(projectName);
    
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/resources/templates/" + templateType,
        QCoreApplication::applicationDirPath() + "/templates/" + templateType,
        QDir::currentPath() + "/resources/templates/" + templateType,
        QDir::currentPath() + "/templates/" + templateType
    };
    
    QString templateSrcPath;
    for (const QString& path : searchPaths) {
        if (QDir(path).exists()) {
            templateSrcPath = path;
            break;
        }
    }
    
    if (templateSrcPath.isEmpty()) {
        QMessageBox::critical(this, "Error", QString("Template directory for '%1' not found.").arg(templateType));
        return;
    }
    
    if (copyDirectoryRecursively(templateSrcPath, finalProjectPath, projectName)) {
        QMessageBox::information(this, "Success", "Project created successfully from template!");
        setProjectRoot(finalProjectPath);
        
        QDir projDir(finalProjectPath);
        QString mainFilePath;
        if (templateType == "cpp") mainFilePath = projDir.absoluteFilePath("main.cpp");
        else if (templateType == "python") mainFilePath = projDir.absoluteFilePath("main.py");
        else if (templateType == "java") mainFilePath = projDir.absoluteFilePath("Main.java");
        else if (templateType == "node") mainFilePath = projDir.absoluteFilePath("index.js");
        else if (templateType == "web") mainFilePath = projDir.absoluteFilePath("index.html");
        
        if (QFile::exists(mainFilePath)) {
            m_editorArea->openFile(mainFilePath);
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to create project from template.");
    }
}
