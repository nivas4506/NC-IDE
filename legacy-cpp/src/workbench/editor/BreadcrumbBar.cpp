// src/workbench/editor/BreadcrumbBar.cpp
#include "BreadcrumbBar.h"
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>
#include <QDir>
#include <QMenu>

BreadcrumbBar::BreadcrumbBar(QWidget* parent) : QWidget(parent) {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 2, 5, 2);
    m_layout->setSpacing(2);
    
    setStyleSheet(
        "QWidget { background-color: #21222c; border-bottom: 1px solid #282a36; }"
        "QPushButton { background-color: transparent; border: none; color: #6272a4; font-size: 9pt; padding: 2px 4px; border-radius: 3px; }"
        "QPushButton:hover { color: #f8f8f2; background-color: #282a36; }"
        "QLabel { color: #6272a4; font-size: 9pt; }"
    );
    
    setFixedHeight(26);
}

void BreadcrumbBar::setFilePath(const QString& filePath, const QString& projectRoot) {
    m_filePath = filePath;
    m_projectRoot = projectRoot;
    
    // Clear layout
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    if (filePath.isEmpty()) {
        QLabel* placeholder = new QLabel("No File Open", this);
        m_layout->addWidget(placeholder);
        m_layout->addStretch(1);
        return;
    }
    
    // Find path relative to projectRoot
    QString displayPath = filePath;
    bool inProject = false;
    if (!projectRoot.isEmpty() && filePath.startsWith(projectRoot)) {
        displayPath = filePath.mid(projectRoot.length());
        if (displayPath.startsWith("/")) displayPath = displayPath.mid(1);
        if (displayPath.startsWith("\\")) displayPath = displayPath.mid(1);
        inProject = true;
    }
    
    QStringList segments;
    if (inProject) {
        segments.append(QFileInfo(projectRoot).fileName());
    }
    
    QStringList parts = displayPath.split(QDir::separator(), Qt::SkipEmptyParts);
    segments.append(parts);
    
    QString currentAccumulatedPath = inProject ? projectRoot : "";
    
    for (int i = 0; i < segments.size(); ++i) {
        if (i > 0) {
            QLabel* arrow = new QLabel(" > ", this);
            m_layout->addWidget(arrow);
            
            if (inProject) {
                currentAccumulatedPath = QDir(currentAccumulatedPath).absoluteFilePath(segments[i]);
            } else {
                if (currentAccumulatedPath.isEmpty()) {
                    currentAccumulatedPath = segments[i];
                } else {
                    currentAccumulatedPath = QDir(currentAccumulatedPath).absoluteFilePath(segments[i]);
                }
            }
        } else {
            if (!inProject) {
                QFileInfo fi(filePath);
                currentAccumulatedPath = fi.absolutePath();
                QStringList absParts = currentAccumulatedPath.split(QDir::separator(), Qt::SkipEmptyParts);
                if (!absParts.isEmpty()) {
                    int idx = currentAccumulatedPath.indexOf(absParts[0]);
                    if (idx >= 0) {
                        currentAccumulatedPath = currentAccumulatedPath.left(idx + absParts[0].length());
                    }
                }
            }
        }
        
        QPushButton* btn = new QPushButton((i == segments.size() - 1 ? "📄 " : "📁 ") + segments[i], this);
        btn->setProperty("path", currentAccumulatedPath);
        btn->setProperty("isFolder", i < segments.size() - 1);
        connect(btn, &QPushButton::clicked, this, &BreadcrumbBar::onSegmentClicked);
        m_layout->addWidget(btn);
    }
    
    m_layout->addStretch(1);
}

void BreadcrumbBar::onSegmentClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QString path = btn->property("path").toString();
    bool isFolder = btn->property("isFolder").toBool();
    
    if (path.isEmpty()) return;
    
    QDir dir;
    if (isFolder) {
        dir.setPath(path);
    } else {
        dir.setPath(QFileInfo(path).absolutePath());
    }
    
    if (!dir.exists()) return;
    
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background-color: #21222c; border: 1px solid #282a36; color: #f8f8f2; }"
        "QMenu::item { padding: 4px 20px 4px 10px; }"
        "QMenu::item:selected { background-color: #44475a; }"
    );
    
    QStringList entryList = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    for (const QString& entry : entryList) {
        QFileInfo fi(dir.absoluteFilePath(entry));
        QAction* act = menu.addAction((fi.isDir() ? "📁 " : "📄 ") + entry);
        act->setData(fi.absoluteFilePath());
    }
    
    if (menu.isEmpty()) {
        menu.addAction("Empty Folder")->setEnabled(false);
    }
    
    QAction* selected = menu.exec(btn->mapToGlobal(QPoint(0, btn->height())));
    if (selected && !selected->data().toString().isEmpty()) {
        QString selectedPath = selected->data().toString();
        if (QFileInfo(selectedPath).isFile()) {
            emit fileSelected(selectedPath);
        }
    }
}
