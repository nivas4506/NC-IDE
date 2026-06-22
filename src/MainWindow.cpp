// src/MainWindow.cpp
#include "MainWindow.h"
#include "settings/SettingsManager.h"
#include "editor/CodeEditor.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCloseEvent>
#include <QFileInfo>
#include <QDebug>
#include <QListWidget>

// Helper to set application-wide Dracula-like styles
static void applyApplicationStyle(const QString& theme) {
    if (theme == "light") {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #f0f0f0; color: #000000; }"
            "QMenuBar { background-color: #e0e0e0; color: #000000; border-bottom: 1px solid #d0d0d0; }"
            "QMenuBar::item:selected { background-color: #d0d0d0; }"
            "QMenu { background-color: #f0f0f0; color: #000000; border: 1px solid #d0d0d0; }"
            "QMenu::item:selected { background-color: #d0d0d0; }"
            "QToolBar { background-color: #e0e0e0; border-bottom: 1px solid #d0d0d0; spacing: 4px; padding: 2px; }"
            "QStatusBar { background-color: #e0e0e0; color: #333333; border-top: 1px solid #d0d0d0; }"
            "QTabWidget::pane { border: 1px solid #c0c0c0; background-color: #f5f5f5; }"
            "QTabBar::tab { background-color: #e0e0e0; color: #555555; border: 1px solid #c0c0c0; border-bottom: none; padding: 6px 12px; margin-right: 2px; border-top-left-radius: 4px; border-top-right-radius: 4px; }"
            "QTabBar::tab:selected { background-color: #f5f5f5; color: #000000; font-weight: bold; }"
            "QDockWidget::title { background-color: #e0e0e0; padding: 4px; border-bottom: 1px solid #d0d0d0; }"
            "QLineEdit { background-color: #ffffff; color: #000000; border: 1px solid #c0c0c0; border-radius: 3px; padding: 3px; }"
            "QPushButton { background-color: #e0e0e0; color: #000000; border: 1px solid #c0c0c0; border-radius: 3px; padding: 4px 8px; }"
            "QPushButton:hover { background-color: #d0d0d0; }"
            "QTreeWidget { background-color: #ffffff; color: #000000; border: 1px solid #c0c0c0; }"
            "QLabel { color: #000000; }"
        );
    } else {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #1e1e24; color: #f8f8f2; }"
            "QMenuBar { background-color: #282a36; color: #f8f8f2; border-bottom: 1px solid #44475a; }"
            "QMenuBar::item:selected { background-color: #44475a; }"
            "QMenu { background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; }"
            "QMenu::item:selected { background-color: #44475a; }"
            "QToolBar { background-color: #282a36; border-bottom: 1px solid #44475a; spacing: 4px; padding: 2px; }"
            "QStatusBar { background-color: #282a36; color: #6272a4; border-top: 1px solid #44475a; }"
            "QTabWidget::pane { border: 1px solid #44475a; background-color: #1e1e24; }"
            "QTabBar::tab { background-color: #282a36; color: #6272a4; border: 1px solid #44475a; border-bottom: none; padding: 6px 12px; margin-right: 2px; border-top-left-radius: 4px; border-top-right-radius: 4px; }"
            "QTabBar::tab:selected { background-color: #1e1e24; color: #f8f8f2; font-weight: bold; }"
            "QDockWidget::title { background-color: #282a36; padding: 4px; border-bottom: 1px solid #44475a; }"
            "QLineEdit { background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; border-radius: 3px; padding: 3px; }"
            "QPushButton { background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; border-radius: 3px; padding: 4px 8px; }"
            "QPushButton:hover { background-color: #6272a4; }"
            "QTreeWidget { background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; }"
            "QCheckBox { color: #f8f8f2; }"
            "QLabel { color: #f8f8f2; }"
        );
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("NC IDE - Native Editor");
    resize(1000, 700);

    // Central widget & manager setup
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setDocumentMode(true);

    m_tabManager = new TabManager(m_tabWidget, this);
    m_searchManager = new SearchManager(this);
    m_projectSearchEngine = new ProjectSearchEngine(this);
    m_sessionManager = new SessionManager(m_tabManager, this);

    // Connections to keep editor context in sync
    connect(m_tabManager, &TabManager::activeFileChanged, this, &MainWindow::onActiveFileChanged);
    connect(m_tabManager, &TabManager::tabDirtyStateChanged, this, [this](int, bool) {
        // Refresh active file display to capture updated title changes
        if (m_tabManager->currentEditor()) {
            onActiveFileChanged(m_tabManager->filePathAt(m_tabWidget->currentIndex()));
        }
    });

    // Create sub panels
    createFindPanel();
    createProjectSearchDock();
    createMenus();
    createToolBar();
    createStatusBarWidget();

    // Central layout containing editor tabs and hidden find panel
    QWidget* centralContainer = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(m_tabWidget);
    centralLayout->addWidget(m_findPanel);
    setCentralWidget(centralContainer);

    // Apply active theme
    SettingsManager& settings = SettingsManager::instance();
    applyApplicationStyle(settings.theme());
    connect(&settings, &SettingsManager::themeChanged, this, [](const QString& theme) {
        applyApplicationStyle(theme);
    });

    // Restore last session
    m_sessionManager->restoreSession();
    
    // Ensure at least one tab is open
    if (m_tabManager->tabCount() == 0) {
        onNewFile();
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent* event) {
    m_sessionManager->saveSession();
    
    // Close all tabs, asking to save if dirty
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        if (!m_tabManager->closeTab(i, true)) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void MainWindow::onNewFile() {
    m_tabManager->newUntitledTab();
}

void MainWindow::onOpenFile() {
    QString path = QFileDialog::getOpenFileName(this, "Open File");
    if (!path.isEmpty()) {
        m_tabManager->openFile(path);
    }
}

void MainWindow::onOpenProject() {
    QString path = QFileDialog::getExistingDirectory(this, "Open Project/Folder");
    if (!path.isEmpty()) {
        m_projectSearchEngine->setProjectRoot(path);
        SettingsManager::instance().addRecentProject(path);
        statusBar()->showMessage("Project Root set to: " + path, 3000);
    }
}

void MainWindow::onSave() {
    int index = m_tabWidget->currentIndex();
    if (index >= 0) {
        m_tabManager->saveTab(index);
    }
}

void MainWindow::onSaveAs() {
    int index = m_tabWidget->currentIndex();
    if (index >= 0) {
        m_tabManager->saveTabAs(index, QString());
    }
}

void MainWindow::onSaveAll() {
    m_tabManager->saveAll();
}

void MainWindow::onCloseTab() {
    int index = m_tabWidget->currentIndex();
    if (index >= 0) {
        m_tabManager->closeTab(index, true);
    }
}

void MainWindow::onCloseAll() {
    m_tabManager->closeAllTabs(true);
}

void MainWindow::onRecentFileTriggered() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        m_tabManager->openFile(action->data().toString());
    }
}

void MainWindow::onUndo() {
    CodeEditor* editor = m_tabManager->currentEditor();
    if (editor) editor->undo();
}

void MainWindow::onRedo() {
    CodeEditor* editor = m_tabManager->currentEditor();
    if (editor) editor->redo();
}

void MainWindow::onGoToLine() {
    CodeEditor* editor = m_tabManager->currentEditor();
    if (!editor) return;
    bool ok;
    int line = QInputDialog::getInt(this, "Go to Line", "Line number:",
                                    editor->textCursor().blockNumber() + 1, 1, editor->blockCount(), 1, &ok);
    if (ok) {
        editor->goToLine(line);
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

    QCheckBox autosaveCheck("Enable Auto-Save Crash Recovery");
    autosaveCheck.setChecked(settings.autoSaveEnabled());

    QSpinBox intervalSpin;
    intervalSpin.setRange(5, 300);
    intervalSpin.setSuffix(" sec");
    intervalSpin.setValue(settings.autoSaveIntervalSeconds());

    form.addRow("Theme:", &themeCombo);
    form.addRow("Editor Font:", &fontCombo);
    form.addRow("Font Size:", &sizeSpin);
    form.addRow(&autosaveCheck);
    form.addRow("Recovery Interval:", &intervalSpin);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        settings.setTheme(themeCombo.currentText());
        settings.setFontFamily(fontCombo.currentText());
        settings.setFontSize(sizeSpin.value());
        settings.setAutoSaveEnabled(autosaveCheck.isChecked());
        settings.setAutoSaveIntervalSeconds(intervalSpin.value());
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
        
        // Populate find box with current editor selection if any
        CodeEditor* editor = m_tabManager->currentEditor();
        if (editor) {
            QString selection = editor->textCursor().selectedText();
            if (!selection.isEmpty() && !selection.contains(QChar::ParagraphSeparator)) {
                m_findInput->setText(selection);
            }
        }
    }
}

void MainWindow::onFindNext() {
    SearchOptions opts;
    opts.matchCase = m_matchCaseCheck->isChecked();
    opts.wholeWord = m_wholeWordCheck->isChecked();
    
    m_searchManager->setActiveEditor(m_tabManager->currentEditor());
    m_searchManager->findNext(m_findInput->text(), opts);
}

void MainWindow::onFindPrev() {
    SearchOptions opts;
    opts.matchCase = m_matchCaseCheck->isChecked();
    opts.wholeWord = m_wholeWordCheck->isChecked();

    m_searchManager->setActiveEditor(m_tabManager->currentEditor());
    m_searchManager->findPrevious(m_findInput->text(), opts);
}

void MainWindow::onReplaceCurrent() {
    SearchOptions opts;
    opts.matchCase = m_matchCaseCheck->isChecked();
    opts.wholeWord = m_wholeWordCheck->isChecked();

    m_searchManager->setActiveEditor(m_tabManager->currentEditor());
    m_searchManager->replaceCurrent(m_findInput->text(), m_replaceInput->text(), opts);
}

void MainWindow::onReplaceAll() {
    SearchOptions opts;
    opts.matchCase = m_matchCaseCheck->isChecked();
    opts.wholeWord = m_wholeWordCheck->isChecked();

    m_searchManager->setActiveEditor(m_tabManager->currentEditor());
    int count = m_searchManager->replaceAll(m_findInput->text(), m_replaceInput->text(), opts);
    statusBar()->showMessage(QString("Replaced %1 occurrences.").arg(count), 3000);
}

void MainWindow::onProjectSearch() {
    m_projectSearchResults->clear();
    QString term = m_projectSearchInput->text();
    if (term.isEmpty()) return;

    bool matchCase = m_projMatchCaseCheck->isChecked();
    bool wholeWord = m_projWholeWordCheck->isChecked();

    QList<ProjectSearchResult> results = m_projectSearchEngine->search(term, matchCase, wholeWord);
    
    // Group results by file
    QMap<QString, QTreeWidgetItem*> fileGroups;
    for (const ProjectSearchResult& res : results) {
        if (!fileGroups.contains(res.filePath)) {
            QTreeWidgetItem* parentItem = new QTreeWidgetItem(m_projectSearchResults);
            parentItem->setText(0, QFileInfo(res.filePath).fileName());
            parentItem->setToolTip(0, res.filePath);
            parentItem->setExpanded(true);
            fileGroups[res.filePath] = parentItem;
        }
        
        QTreeWidgetItem* childItem = new QTreeWidgetItem(fileGroups[res.filePath]);
        childItem->setText(0, QString("Line %1: %2").arg(res.lineNumber).arg(res.lineText.trimmed()));
        childItem->setData(0, Qt::UserRole, res.filePath);
        childItem->setData(0, Qt::UserRole + 1, res.lineNumber);
        childItem->setData(0, Qt::UserRole + 2, res.columnNumber);
    }
}

void MainWindow::onProjectSearchResultDoubleClicked(QTreeWidgetItem* item, int) {
    if (!item || item->parent() == nullptr) return; // parent item is just filename

    QString filePath = item->data(0, Qt::UserRole).toString();
    int line = item->data(0, Qt::UserRole + 1).toInt();
    
    CodeEditor* editor = m_tabManager->openFile(filePath);
    if (editor) {
        editor->goToLine(line);
        editor->setFocus();
    }
}

void MainWindow::updateStatusBar(const QString& filePath) {
    if (filePath.isEmpty()) {
        m_statusFilePath->setText("No File Open");
        m_statusLanguage->setText("PlainText");
        m_statusCursorPos->setText("Ln 1, Col 1");
    } else {
        m_statusFilePath->setText(filePath);
        CodeEditor* editor = m_tabManager->currentEditor();
        if (editor) {
            Language lang = editor->language();
            QString langStr = "PlainText";
            if (lang == Language::C) langStr = "C";
            else if (lang == Language::Cpp) langStr = "C++";
            else if (lang == Language::Python) langStr = "Python";
            else if (lang == Language::Java) langStr = "Java";
            else if (lang == Language::JavaScript) langStr = "JavaScript";
            else if (lang == Language::Html) langStr = "HTML";
            else if (lang == Language::Css) langStr = "CSS";
            
            m_statusLanguage->setText(langStr);
            m_statusCursorPos->setText(editor->cursorPositionLabel());
        }
    }
}

void MainWindow::onActiveFileChanged(const QString& filePath) {
    updateStatusBar(filePath);
    CodeEditor* editor = m_tabManager->currentEditor();
    if (editor) {
        disconnect(editor, &CodeEditor::cursorPositionChanged2, this, &MainWindow::onCursorPositionChanged);
        connect(editor, &CodeEditor::cursorPositionChanged2, this, &MainWindow::onCursorPositionChanged);
    }
    updateRecentFilesMenu();
}

void MainWindow::onCursorPositionChanged(const QString& label) {
    m_statusCursorPos->setText(label);
}

void MainWindow::createMenus() {
    QMenuBar* bar = menuBar();

    // File
    QMenu* fileMenu = bar->addMenu("&File");
    fileMenu->addAction("&New File", this, &MainWindow::onNewFile, QKeySequence::New);
    fileMenu->addAction("&Open File...", this, &MainWindow::onOpenFile, QKeySequence::Open);
    fileMenu->addAction("Open Folder/&Project...", this, &MainWindow::onOpenProject);
    
    m_recentFilesMenu = fileMenu->addMenu("Recent Files");
    updateRecentFilesMenu();

    fileMenu->addSeparator();
    fileMenu->addAction("&Save", this, &MainWindow::onSave, QKeySequence::Save);
    fileMenu->addAction("Save &As...", this, &MainWindow::onSaveAs, QKeySequence::SaveAs);
    fileMenu->addAction("Save Al&l", this, &MainWindow::onSaveAll, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S));
    
    fileMenu->addSeparator();
    fileMenu->addAction("&Close Tab", this, &MainWindow::onCloseTab, QKeySequence::Close);
    fileMenu->addAction("Close &All Tabs", this, &MainWindow::onCloseAll, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
    
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &MainWindow::close, QKeySequence::Quit);

    // Edit
    QMenu* editMenu = bar->addMenu("&Edit");
    editMenu->addAction("&Undo", this, &MainWindow::onUndo, QKeySequence::Undo);
    editMenu->addAction("&Redo", this, &MainWindow::onRedo, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("&Preferences...", this, &MainWindow::onShowPreferences);

    // Search
    QMenu* searchMenu = bar->addMenu("&Search");
    searchMenu->addAction("&Find...", this, [this]() { onToggleFindPanel(false); }, QKeySequence::Find);
    searchMenu->addAction("Find & Replace...", this, [this]() { onToggleFindPanel(true); }, QKeySequence(Qt::CTRL | Qt::Key_H));
    searchMenu->addAction("Find in &Files (Project)...", this, [this]() {
        m_projectSearchDock->show();
        m_projectSearchInput->setFocus();
    }, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F));
    searchMenu->addAction("&Go to Line...", this, &MainWindow::onGoToLine, QKeySequence(Qt::CTRL | Qt::Key_G));

    // View
    QMenu* viewMenu = bar->addMenu("&View");
    viewMenu->addAction(m_projectSearchDock->toggleViewAction());
}

void MainWindow::createToolBar() {
    QToolBar* toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(false);
    toolbar->addAction("New", this, &MainWindow::onNewFile);
    toolbar->addAction("Open", this, &MainWindow::onOpenFile);
    toolbar->addAction("Save", this, &MainWindow::onSave);
    toolbar->addAction("Save All", this, &MainWindow::onSaveAll);
    toolbar->addSeparator();
    toolbar->addAction("Find", this, [this]() { onToggleFindPanel(false); });
    toolbar->addAction("Replace", this, [this]() { onToggleFindPanel(true); });
    toolbar->addAction("Project Search", this, [this]() { m_projectSearchDock->show(); });
    toolbar->addSeparator();
    toolbar->addAction("Preferences", this, &MainWindow::onShowPreferences);
}

void MainWindow::createStatusBarWidget() {
    QStatusBar* bar = statusBar();
    m_statusFilePath = new QLabel("No File Open", this);
    m_statusCursorPos = new QLabel("Ln 1, Col 1", this);
    m_statusLanguage = new QLabel("PlainText", this);

    bar->addWidget(m_statusFilePath, 1);
    bar->addPermanentWidget(m_statusLanguage);
    bar->addPermanentWidget(m_statusCursorPos);
}

void MainWindow::createFindPanel() {
    m_findPanel = new QWidget(this);
    m_findPanel->setObjectName("FindPanel");
    m_findPanel->hide();

    QVBoxLayout* mainLayout = new QVBoxLayout(m_findPanel);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);

    // Row 1: Find Text Box & Control buttons
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

    // Row 2: Replace Text Box & Action buttons
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
    
    // Fill empty spacing to match checks above
    replaceLayout->addSpacing(250);

    mainLayout->addWidget(findRowWidget);
    mainLayout->addWidget(m_replaceRowWidget);

    // Connections
    connect(m_findInput, &QLineEdit::returnPressed, this, &MainWindow::onFindNext);
    connect(m_findNextBtn, &QPushButton::clicked, this, &MainWindow::onFindNext);
    connect(m_findPrevBtn, &QPushButton::clicked, this, &MainWindow::onFindPrev);
    connect(m_replaceBtn, &QPushButton::clicked, this, &MainWindow::onReplaceCurrent);
    connect(m_replaceAllBtn, &QPushButton::clicked, this, &MainWindow::onReplaceAll);
    connect(closeBtn, &QPushButton::clicked, m_findPanel, &QWidget::hide);
}

void MainWindow::createProjectSearchDock() {
    m_projectSearchDock = new QDockWidget("Project Search", this);
    m_projectSearchDock->setObjectName("ProjectSearchDock");
    m_projectSearchDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* inner = new QWidget(m_projectSearchDock);
    QVBoxLayout* layout = new QVBoxLayout(inner);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);

    m_projectSearchInput = new QLineEdit(inner);
    m_projectSearchInput->setPlaceholderText("Search terms...");
    layout->addWidget(m_projectSearchInput);

    QHBoxLayout* options = new QHBoxLayout();
    m_projMatchCaseCheck = new QCheckBox("Match Case", inner);
    m_projWholeWordCheck = new QCheckBox("Whole Word", inner);
    m_projectSearchBtn = new QPushButton("Search", inner);
    
    options->addWidget(m_projMatchCaseCheck);
    options->addWidget(m_projWholeWordCheck);
    options->addWidget(m_projectSearchBtn);
    layout->addLayout(options);

    m_projectSearchResults = new QTreeWidget(inner);
    m_projectSearchResults->setColumnCount(1);
    m_projectSearchResults->setHeaderHidden(true);
    layout->addWidget(m_projectSearchResults, 1);

    m_projectSearchDock->setWidget(inner);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectSearchDock);

    connect(m_projectSearchInput, &QLineEdit::returnPressed, this, &MainWindow::onProjectSearch);
    connect(m_projectSearchBtn, &QPushButton::clicked, this, &MainWindow::onProjectSearch);
    connect(m_projectSearchResults, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onProjectSearchResultDoubleClicked);
}

void MainWindow::updateRecentFilesMenu() {
    m_recentFilesMenu->clear();
    QStringList files = SettingsManager::instance().recentFiles();
    if (files.isEmpty()) {
        QAction* action = m_recentFilesMenu->addAction("No Recent Files");
        action->setEnabled(false);
        return;
    }

    for (const QString& file : files) {
        QAction* action = m_recentFilesMenu->addAction(QFileInfo(file).fileName());
        action->setData(file);
        action->setToolTip(file);
        connect(action, &QAction::triggered, this, &MainWindow::onRecentFileTriggered);
    }
}
