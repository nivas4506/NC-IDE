// src/workbench/editor/EditorGroup.cpp
#include "EditorGroup.h"
#include <QVBoxLayout>
#include <QEvent>
#include <QFocusEvent>

EditorGroup::EditorGroup(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_breadcrumbBar = new BreadcrumbBar(this);
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    layout->addWidget(m_breadcrumbBar);
    layout->addWidget(m_tabWidget);

    m_tabManager = new TabManager(m_tabWidget, this);

    connect(m_tabManager, &TabManager::activeFileChanged, this, &EditorGroup::onActiveFileChanged);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &EditorGroup::onTabChanged);

    connect(m_breadcrumbBar, &BreadcrumbBar::fileSelected, this, [this](const QString& path) {
        m_tabManager->openFile(path);
    });

    m_tabWidget->installEventFilter(this);
    
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; background-color: #1e1e24; }"
        "QTabBar::tab { background-color: #191a21; color: #6272a4; border: 1px solid #282a36; border-bottom: none; padding: 6px 12px; margin-right: 1px; }"
        "QTabBar::tab:selected { background-color: #21222c; color: #f8f8f2; font-weight: bold; border-top: 2px solid #50fa7b; }"
        "QTabBar::tab:hover { color: #f8f8f2; background-color: #21222c; }"
    );

    setActive(false);
}

EditorGroup::~EditorGroup() {}

CodeEditor* EditorGroup::currentEditor() const {
    return m_tabManager->currentEditor();
}

QString EditorGroup::currentFilePath() const {
    if (m_tabWidget->currentIndex() >= 0) {
        return m_tabManager->filePathAt(m_tabWidget->currentIndex());
    }
    return QString();
}

void EditorGroup::setProjectRoot(const QString& root) {
    m_projectRoot = root;
    m_breadcrumbBar->setFilePath(currentFilePath(), root);
}

void EditorGroup::onActiveFileChanged(const QString& path) {
    m_breadcrumbBar->setFilePath(path, m_projectRoot);
    emit activeFileChanged(path);
}

void EditorGroup::onTabChanged(int index) {
    if (index >= 0) {
        QString path = m_tabManager->filePathAt(index);
        m_breadcrumbBar->setFilePath(path, m_projectRoot);
        
        if (CodeEditor* ed = m_tabManager->editorAt(index)) {
            ed->installEventFilter(this);
        }
    } else {
        m_breadcrumbBar->setFilePath("", m_projectRoot);
    }
    emit editorGroupFocused(this);
}

void EditorGroup::setActive(bool active) {
    m_isActive = active;
    if (active) {
        m_breadcrumbBar->setStyleSheet(
            "QWidget { background-color: #282a36; border-bottom: 2px solid #50fa7b; }"
            "QPushButton { background-color: transparent; border: none; color: #f8f8f2; font-size: 9pt; padding: 2px 4px; border-radius: 3px; }"
            "QPushButton:hover { background-color: #44475a; }"
            "QLabel { color: #8be9fd; font-size: 9pt; }"
        );
    } else {
        m_breadcrumbBar->setStyleSheet(
            "QWidget { background-color: #21222c; border-bottom: 1px solid #282a36; }"
            "QPushButton { background-color: transparent; border: none; color: #6272a4; font-size: 9pt; padding: 2px 4px; border-radius: 3px; }"
            "QPushButton:hover { color: #f8f8f2; background-color: #282a36; }"
            "QLabel { color: #6272a4; font-size: 9pt; }"
        );
    }
}

bool EditorGroup::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::MouseButtonPress) {
        emit editorGroupFocused(this);
    }
    return QWidget::eventFilter(obj, event);
}
