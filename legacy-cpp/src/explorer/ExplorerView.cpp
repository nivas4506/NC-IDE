// src/explorer/ExplorerView.cpp
#include "ExplorerView.h"
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QSet>

ExplorerView::ExplorerView(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_treeWidget->setStyleSheet(
        "QTreeWidget { background-color: #21222c; border: none; color: #f8f8f2; font-size: 10pt; }"
        "QTreeWidget::item { padding: 4px; }"
        "QTreeWidget::item:hover { background-color: #282a36; }"
        "QTreeWidget::item:selected { background-color: #44475a; color: #50fa7b; }"
    );

    layout->addWidget(m_treeWidget);

    connect(m_treeWidget, &QTreeWidget::itemExpanded, this, &ExplorerView::onItemExpanded);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &ExplorerView::onItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &ExplorerView::showContextMenu);
}

void ExplorerView::addRootFolder(const QString& path) {
    if (path.isEmpty() || m_rootPaths.contains(path)) return;
    
    m_rootPaths.append(path);
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_treeWidget);
    rootItem->setText(0, "📁 " + QFileInfo(path).fileName());
    rootItem->setToolTip(0, path);
    rootItem->setData(0, Qt::UserRole, path);
    rootItem->setData(0, Qt::UserRole + 1, true);

    QTreeWidgetItem* dummy = new QTreeWidgetItem(rootItem);
    dummy->setText(0, "Loading...");

    m_treeWidget->addTopLevelItem(rootItem);
}

void ExplorerView::clearWorkspace() {
    m_treeWidget->clear();
    m_rootPaths.clear();
}

QString ExplorerView::getItemPath(QTreeWidgetItem* item) const {
    if (!item) return QString();
    return item->data(0, Qt::UserRole).toString();
}

void ExplorerView::onItemExpanded(QTreeWidgetItem* item) {
    if (!item) return;
    
    if (item->childCount() == 1 && item->child(0)->text(0) == "Loading...") {
        delete item->child(0);
        QString path = getItemPath(item);
        populateItem(item, path);
    }
}

void ExplorerView::populateItem(QTreeWidgetItem* parentItem, const QString& path) {
    QDir dir(path);
    if (!dir.exists()) return;

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    for (const QFileInfo& fi : entries) {
        QTreeWidgetItem* child = new QTreeWidgetItem(parentItem);
        if (fi.isDir()) {
            child->setText(0, "📁 " + fi.fileName());
            child->setData(0, Qt::UserRole + 1, true);
            
            QTreeWidgetItem* dummy = new QTreeWidgetItem(child);
            dummy->setText(0, "Loading...");
        } else {
            child->setText(0, "📄 " + fi.fileName());
            child->setData(0, Qt::UserRole + 1, false);
        }
        child->setData(0, Qt::UserRole, fi.absoluteFilePath());
    }
}

void ExplorerView::onItemDoubleClicked(QTreeWidgetItem* item, int) {
    if (!item) return;
    bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
    if (!isFolder) {
        emit fileSelected(getItemPath(item));
    }
}

void ExplorerView::showContextMenu(const QPoint& pos) {
    QTreeWidgetItem* item = m_treeWidget->itemAt(pos);
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background-color: #21222c; border: 1px solid #282a36; color: #f8f8f2; }"
        "QMenu::item { padding: 4px 20px 4px 10px; }"
        "QMenu::item:selected { background-color: #44475a; }"
    );

    if (item) {
        bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
        menu.addAction("New File...", this, &ExplorerView::onCreateFile);
        menu.addAction("New Folder...", this, &ExplorerView::onCreateFolder);
        menu.addSeparator();
        menu.addAction("Rename...", this, &ExplorerView::onRenameItem);
        menu.addAction("Delete", this, &ExplorerView::onDeleteItem);
        menu.addSeparator();
        menu.addAction("Copy", this, &ExplorerView::onCopyItem);
        if (!m_clipboardPath.isEmpty() && isFolder) {
            menu.addAction("Paste", this, &ExplorerView::onPasteItem);
        }
        menu.addSeparator();
        menu.addAction("Refresh", this, &ExplorerView::onRefresh);
    } else {
        menu.addAction("Add Folder to Workspace...", this, [this]() {
            QString path = QFileDialog::getExistingDirectory(this, "Add Folder to Workspace");
            if (!path.isEmpty()) {
                addRootFolder(path);
            }
        });
        if (!m_clipboardPath.isEmpty() && !m_rootPaths.isEmpty()) {
            menu.addAction("Paste to Workspace Root", this, &ExplorerView::onPasteItem);
        }
        menu.addAction("Refresh All", this, &ExplorerView::onRefresh);
    }

    menu.exec(m_treeWidget->mapToGlobal(pos));
}

void ExplorerView::onCreateFile() {
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;

    QString parentPath = getItemPath(item);
    bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
    if (!isFolder) {
        parentPath = QFileInfo(parentPath).absolutePath();
    }

    bool ok;
    QString fileName = QInputDialog::getText(this, "New File", "File Name:", QLineEdit::Normal, "", &ok);
    if (!ok || fileName.isEmpty()) return;

    QString finalPath = QDir(parentPath).absoluteFilePath(fileName);
    QFile file(finalPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.close();
        onRefresh();
    } else {
        QMessageBox::critical(this, "Error", "Failed to create file.");
    }
}

void ExplorerView::onCreateFolder() {
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;

    QString parentPath = getItemPath(item);
    bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
    if (!isFolder) {
        parentPath = QFileInfo(parentPath).absolutePath();
    }

    bool ok;
    QString folderName = QInputDialog::getText(this, "New Folder", "Folder Name:", QLineEdit::Normal, "", &ok);
    if (!ok || folderName.isEmpty()) return;

    QString finalPath = QDir(parentPath).absoluteFilePath(folderName);
    QDir dir;
    if (dir.mkdir(finalPath)) {
        onRefresh();
    } else {
        QMessageBox::critical(this, "Error", "Failed to create folder.");
    }
}

void ExplorerView::onRenameItem() {
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;

    QString path = getItemPath(item);
    QFileInfo fi(path);
    
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", "New Name:", QLineEdit::Normal, fi.fileName(), &ok);
    if (!ok || newName.isEmpty() || newName == fi.fileName()) return;

    QString newPath = fi.absoluteDir().absoluteFilePath(newName);
    
    if (QFile::rename(path, newPath) || QDir().rename(path, newPath)) {
        onRefresh();
    } else {
        QMessageBox::critical(this, "Error", "Rename failed.");
    }
}

void ExplorerView::onDeleteItem() {
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;

    QString path = getItemPath(item);
    bool isFolder = item->data(0, Qt::UserRole + 1).toBool();

    auto reply = QMessageBox::question(this, "Delete", 
        QString("Are you sure you want to delete '%1'?").arg(QFileInfo(path).fileName()),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool ok = false;
        if (isFolder) {
            ok = QDir(path).removeRecursively();
        } else {
            ok = QFile::remove(path);
        }

        if (ok) {
            onRefresh();
        } else {
            QMessageBox::critical(this, "Error", "Delete failed.");
        }
    }
}

void ExplorerView::onCopyItem() {
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;
    m_clipboardPath = getItemPath(item);
}

void ExplorerView::onPasteItem() {
    if (m_clipboardPath.isEmpty()) return;

    QTreeWidgetItem* item = m_treeWidget->currentItem();
    QString destDir;
    if (item) {
        destDir = getItemPath(item);
        bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
        if (!isFolder) {
            destDir = QFileInfo(destDir).absolutePath();
        }
    } else if (!m_rootPaths.isEmpty()) {
        destDir = m_rootPaths.first();
    }

    if (destDir.isEmpty()) return;

    QFileInfo srcInfo(m_clipboardPath);
    QString destPath = QDir(destDir).absoluteFilePath(srcInfo.fileName());

    bool success = false;
    if (srcInfo.isDir()) {
        copyDirectoryRecursively(m_clipboardPath, destPath);
        success = true;
    } else {
        success = QFile::copy(m_clipboardPath, destPath);
    }

    if (success) {
        onRefresh();
    } else {
        QMessageBox::critical(this, "Error", "Paste failed.");
    }
}

void ExplorerView::copyDirectoryRecursively(const QString& src, const QString& dest) {
    QDir().mkpath(dest);
    QDir srcDir(src);
    for (const QString& file : srcDir.entryList(QDir::Files)) {
        QFile::copy(srcDir.absoluteFilePath(file), QDir(dest).absoluteFilePath(file));
    }
    for (const QString& dir : srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        copyDirectoryRecursively(srcDir.absoluteFilePath(dir), QDir(dest).absoluteFilePath(dir));
    }
}

void ExplorerView::onRefresh() {
    QSet<QString> expandedPaths;
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* top = m_treeWidget->topLevelItem(i);
        if (top->isExpanded()) {
            expandedPaths.insert(getItemPath(top));
        }
    }
    
    QStringList currentRoots = m_rootPaths;
    clearWorkspace();
    
    for (const QString& root : currentRoots) {
        addRootFolder(root);
    }
    
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* top = m_treeWidget->topLevelItem(i);
        if (expandedPaths.contains(getItemPath(top))) {
            top->setExpanded(true);
        }
    }
}
