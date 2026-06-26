// src/scm/SourceControlView.cpp
#include "SourceControlView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QFileInfo>
#include <QDebug>

SourceControlView::SourceControlView(GitManager* gitManager, QWidget* parent)
    : QWidget(parent), m_gitManager(gitManager) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_stack = new QStackedWidget(this);
    mainLayout->addWidget(m_stack);
    
    createNoGitWidget();
    createNotRepoWidget();
    createMainWidget();
    
    m_stack->addWidget(m_noGitWidget);  // Index 0: No Git in PATH
    m_stack->addWidget(m_notRepoWidget); // Index 1: Not a repo
    m_stack->addWidget(m_mainWidget);    // Index 2: Main Git View
    
    // Connect GitManager signals
    connect(m_gitManager, &GitManager::statusChanged, this, &SourceControlView::onStatusChanged);
    connect(m_gitManager, &GitManager::branchesChanged, this, &SourceControlView::onBranchesChanged);
    connect(m_gitManager, &GitManager::isGitRepo, this, &SourceControlView::onIsGitRepo);
    connect(m_gitManager, &GitManager::operationFailed, this, &SourceControlView::onOperationFailed);
    
    // Initial display state
    m_stack->setCurrentWidget(m_notRepoWidget);
}

SourceControlView::~SourceControlView() {
}

void SourceControlView::createNoGitWidget() {
    m_noGitWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_noGitWidget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);
    
    QLabel* warningIcon = new QLabel("⚠️", m_noGitWidget);
    warningIcon->setStyleSheet("font-size: 36px;");
    warningIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(warningIcon);
    
    QLabel* label = new QLabel("Git executable not found in PATH.", m_noGitWidget);
    label->setStyleSheet("font-weight: bold; color: #ff5555;"); // Dracula red
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);
    
    QLabel* instructions = new QLabel("Please install Git and ensure it is in your system's PATH environment variable, then restart NC IDE.", m_noGitWidget);
    instructions->setStyleSheet("color: #6272a4; font-size: 10pt;");
    instructions->setAlignment(Qt::AlignCenter);
    instructions->setWordWrap(true);
    layout->addWidget(instructions);
}

void SourceControlView::createNotRepoWidget() {
    m_notRepoWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_notRepoWidget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);
    
    QLabel* label = new QLabel("No Git repository detected in this folder.", m_notRepoWidget);
    label->setStyleSheet("font-weight: bold; color: #6272a4;");
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);
    
    QPushButton* initBtn = new QPushButton("Initialize Repository", m_notRepoWidget);
    initBtn->setStyleSheet(
        "background-color: #50fa7b; color: #282a36; border: none; font-weight: bold; padding: 8px 16px; border-radius: 4px;"
    );
    connect(initBtn, &QPushButton::clicked, this, &SourceControlView::onInitClicked);
    layout->addWidget(initBtn);
}

void SourceControlView::createMainWidget() {
    m_mainWidget = new QWidget(this);
    m_mainWidget->setStyleSheet(
        "QWidget { background-color: #21222c; }"
        "QTreeWidget { background-color: #21222c; border: none; color: #f8f8f2; }"
        "QTreeWidget::item { padding: 4px; }"
        "QTreeWidget::item:hover { background-color: #282a36; }"
        "QTreeWidget::item:selected { background-color: #44475a; }"
    );
    QVBoxLayout* layout = new QVBoxLayout(m_mainWidget);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(6);
    
    // Header row (Branch and Sync buttons)
    QWidget* header = new QWidget(m_mainWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(4, 4, 4, 4);
    headerLayout->setSpacing(6);
    
    m_branchLabel = new QLabel("Branch: main", header);
    m_branchLabel->setStyleSheet("font-weight: bold; color: #8be9fd;");
    headerLayout->addWidget(m_branchLabel, 1);
    
    m_pullBtn = new QPushButton("Pull", header);
    m_pullBtn->setToolTip("Pull from origin (fast-forward only)");
    m_pullBtn->setStyleSheet("background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; padding: 3px 8px; border-radius: 3px;");
    connect(m_pullBtn, &QPushButton::clicked, this, &SourceControlView::onPullClicked);
    headerLayout->addWidget(m_pullBtn);
    
    m_pushBtn = new QPushButton("Push", header);
    m_pushBtn->setToolTip("Push to origin");
    m_pushBtn->setStyleSheet("background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; padding: 3px 8px; border-radius: 3px;");
    connect(m_pushBtn, &QPushButton::clicked, this, &SourceControlView::onPushClicked);
    headerLayout->addWidget(m_pushBtn);
    
    layout->addWidget(header);
    
    // Changes list (Tree Widget)
    m_treeWidget = new QTreeWidget(m_mainWidget);
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_treeWidget, 1);
    
    m_stagedHeader = new QTreeWidgetItem(m_treeWidget);
    m_stagedHeader->setText(0, "Staged Changes");
    m_stagedHeader->setExpanded(true);
    QFont boldFont = m_stagedHeader->font(0);
    boldFont.setBold(true);
    m_stagedHeader->setFont(0, boldFont);
    
    m_changesHeader = new QTreeWidgetItem(m_treeWidget);
    m_changesHeader->setText(0, "Changes");
    m_changesHeader->setExpanded(true);
    m_changesHeader->setFont(0, boldFont);
    
    // Commit message section
    QWidget* commitSection = new QWidget(m_mainWidget);
    QVBoxLayout* commitLayout = new QVBoxLayout(commitSection);
    commitLayout->setContentsMargins(4, 4, 4, 4);
    commitLayout->setSpacing(4);
    
    m_commitMsgEdit = new QTextEdit(commitSection);
    m_commitMsgEdit->setPlaceholderText("Message (Ctrl+Enter to commit)...");
    m_commitMsgEdit->setMaximumHeight(70);
    m_commitMsgEdit->setStyleSheet(
        "background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; border-radius: 4px;"
    );
    connect(m_commitMsgEdit, &QTextEdit::textChanged, this, &SourceControlView::onCommitMessageChanged);
    commitLayout->addWidget(m_commitMsgEdit);
    
    m_commitBtn = new QPushButton("Commit", commitSection);
    m_commitBtn->setStyleSheet(
        "background-color: #50fa7b; color: #282a36; border: none; font-weight: bold; padding: 6px; border-radius: 4px;"
    );
    m_commitBtn->setEnabled(false);
    connect(m_commitBtn, &QPushButton::clicked, this, &SourceControlView::onCommitClicked);
    commitLayout->addWidget(m_commitBtn);
    
    layout->addWidget(commitSection);
    
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &SourceControlView::onTreeItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &SourceControlView::onCustomContextMenu);
}

void SourceControlView::onStatusChanged(const QVector<GitFileStatus>& status) {
    m_currentStatus = status;
    refreshTree();
    onCommitMessageChanged(); // refresh commit button state
}

void SourceControlView::onBranchesChanged(const QStringList& /* branches */, const QString& currentBranch) {
    m_branchLabel->setText("Branch: " + (currentBranch.isEmpty() ? "DETACHED" : currentBranch));
}

void SourceControlView::onIsGitRepo(bool status) {
    if (status) {
        m_stack->setCurrentWidget(m_mainWidget);
    } else {
        m_stack->setCurrentWidget(m_notRepoWidget);
    }
}

void SourceControlView::onOperationFailed(const QString& op, const QString& stderrText) {
    if (stderrText.contains("not found in PATH") || stderrText.contains("executable not found")) {
        m_stack->setCurrentWidget(m_noGitWidget);
    } else {
        qDebug() << "Git operation failed:" << op << stderrText;
    }
}

void SourceControlView::onCommitClicked() {
    QString msg = m_commitMsgEdit->toPlainText().trimmed();
    if (!msg.isEmpty()) {
        m_gitManager->commitChanges(msg);
        m_commitMsgEdit->clear();
    }
}

void SourceControlView::onPullClicked() {
    m_gitManager->pull();
}

void SourceControlView::onPushClicked() {
    m_gitManager->push();
}

void SourceControlView::onInitClicked() {
    m_gitManager->initRepository();
}

void SourceControlView::onCommitMessageChanged() {
    QString msg = m_commitMsgEdit->toPlainText().trimmed();
    bool hasAnyChanges = !m_currentStatus.isEmpty();
    m_commitBtn->setEnabled(!msg.isEmpty() && hasAnyChanges);
}

void SourceControlView::refreshTree() {
    qDeleteAll(m_stagedHeader->takeChildren());
    qDeleteAll(m_changesHeader->takeChildren());
    
    for (const GitFileStatus& fs : m_currentStatus) {
        QTreeWidgetItem* parent = fs.staged ? m_stagedHeader : m_changesHeader;
        QTreeWidgetItem* item = new QTreeWidgetItem(parent);
        
        QFileInfo fi(fs.path);
        QString stateChar = "?";
        QColor stateColor = QColor("#6272a4"); // Default muted
        
        if (fs.state == "Untracked") {
            stateChar = "U";
            stateColor = QColor("#50fa7b"); // Green
        } else if (fs.state == "Added") {
            stateChar = "A";
            stateColor = QColor("#50fa7b"); // Green
        } else if (fs.state == "Modified") {
            stateChar = "M";
            stateColor = QColor("#ffb86c"); // Warning Orange
        } else if (fs.state == "Renamed") {
            stateChar = "R";
            stateColor = QColor("#ffb86c");
        } else if (fs.state == "Deleted") {
            stateChar = "D";
            stateColor = QColor("#ff5555"); // Red
        } else if (fs.state == "Conflicted") {
            stateChar = "C";
            stateColor = QColor("#ff5555"); // Red
        }
        
        item->setText(0, QString("%1 (%2)").arg(fi.fileName()).arg(stateChar));
        item->setToolTip(0, fs.path);
        item->setForeground(0, QBrush(stateColor));
        item->setData(0, Qt::UserRole, fs.path);
        item->setData(0, Qt::UserRole + 1, fs.staged);
    }
    
    m_stagedHeader->setExpanded(m_stagedHeader->childCount() > 0);
    m_changesHeader->setExpanded(m_changesHeader->childCount() > 0);
}

void SourceControlView::onTreeItemDoubleClicked(QTreeWidgetItem* item, int /* column */) {
    if (!item || item == m_stagedHeader || item == m_changesHeader) return;
    
    QVariant pathVal = item->data(0, Qt::UserRole);
    if (pathVal.isValid()) {
        emit fileSelected(pathVal.toString());
    }
}

void SourceControlView::onCustomContextMenu(const QPoint& point) {
    QTreeWidgetItem* item = m_treeWidget->itemAt(point);
    if (!item || item == m_stagedHeader || item == m_changesHeader) return;
    
    QVariant pathVal = item->data(0, Qt::UserRole);
    if (!pathVal.isValid()) return;
    
    QString relPath = pathVal.toString();
    bool isStaged = item->data(0, Qt::UserRole + 1).toBool();
    
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background-color: #21222c; border: 1px solid #282a36; color: #f8f8f2; }"
        "QMenu::item { padding: 4px 20px 4px 10px; }"
        "QMenu::item:selected { background-color: #44475a; }"
    );
    QAction* diffAction = menu.addAction("Open Diff");
    QAction* stageAction = isStaged ? menu.addAction("Unstage File") : menu.addAction("Stage File");
    
    QAction* selected = menu.exec(m_treeWidget->mapToGlobal(point));
    if (selected == diffAction) {
        emit fileSelected(relPath);
    } else if (selected == stageAction) {
        if (isStaged) {
            m_gitManager->unstageFile(relPath);
        } else {
            m_gitManager->stageFile(relPath);
        }
    }
}
