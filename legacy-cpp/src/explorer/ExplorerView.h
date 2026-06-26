// src/explorer/ExplorerView.h
#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QMenu>

class ExplorerView : public QWidget {
    Q_OBJECT
public:
    explicit ExplorerView(QWidget* parent = nullptr);

    void addRootFolder(const QString& path);
    void clearWorkspace();
    QStringList rootFolders() const { return m_rootPaths; }

signals:
    void fileSelected(const QString& path);

private slots:
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void showContextMenu(const QPoint& pos);
    
    // File operations
    void onCreateFile();
    void onCreateFolder();
    void onRenameItem();
    void onDeleteItem();
    void onCopyItem();
    void onPasteItem();
    void onRefresh();

private:
    QTreeWidget* m_treeWidget;
    QStringList m_rootPaths;
    QString m_clipboardPath;
    bool m_isCutOperation = false;

    void populateItem(QTreeWidgetItem* parentItem, const QString& path);
    QString getItemPath(QTreeWidgetItem* item) const;
    void copyDirectoryRecursively(const QString& src, const QString& dest);
};
