// src/scm/SourceControlView.h
#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include "GitManager.h"

class SourceControlView : public QWidget {
    Q_OBJECT
public:
    explicit SourceControlView(GitManager* gitManager, QWidget* parent = nullptr);
    ~SourceControlView();
    
signals:
    void fileSelected(const QString& relativePath);

private slots:
    void onStatusChanged(const QVector<GitFileStatus>& status);
    void onBranchesChanged(const QStringList& branches, const QString& currentBranch);
    void onIsGitRepo(bool status);
    void onOperationFailed(const QString& op, const QString& stderrText);
    
    void onCommitClicked();
    void onPullClicked();
    void onPushClicked();
    void onInitClicked();
    
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onCommitMessageChanged();
    void onCustomContextMenu(const QPoint& point);

private:
    GitManager* m_gitManager;
    
    // Stacked widget for states
    QStackedWidget* m_stack;
    
    // Warning views
    QWidget* m_noGitWidget;
    QWidget* m_notRepoWidget;
    
    // Main Source Control view
    QWidget* m_mainWidget;
    QLabel* m_branchLabel;
    QPushButton* m_pullBtn;
    QPushButton* m_pushBtn;
    QTreeWidget* m_treeWidget;
    QTextEdit* m_commitMsgEdit;
    QPushButton* m_commitBtn;
    
    QTreeWidgetItem* m_stagedHeader;
    QTreeWidgetItem* m_changesHeader;
    
    QVector<GitFileStatus> m_currentStatus;
    
    void createNoGitWidget();
    void createNotRepoWidget();
    void createMainWidget();
    void refreshTree();
};
