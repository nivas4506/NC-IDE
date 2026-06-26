// src/workbench/editor/EditorArea.cpp
#include "EditorArea.h"
#include <QVBoxLayout>
#include <QTabBar>

EditorArea::EditorArea(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_mainSplitter = new QSplitter(this);
    m_mainSplitter->setChildrenCollapsible(false);
    layout->addWidget(m_mainSplitter);

    // Default first pane
    EditorGroup* firstGroup = new EditorGroup(this);
    addGroup(firstGroup);
    setActiveGroup(firstGroup);
}

EditorArea::~EditorArea() {}

void EditorArea::addGroup(EditorGroup* group) {
    m_groups.append(group);
    m_mainSplitter->addWidget(group);
    group->setProjectRoot(m_projectRoot);

    connect(group, &EditorGroup::editorGroupFocused, this, &EditorArea::handleGroupFocused);
    connect(group, &EditorGroup::activeFileChanged, this, &EditorArea::handleGroupFileChanged);
    
    connect(group->tabWidget(), &QTabWidget::tabCloseRequested, this, [this, group](int index) {
        group->tabManager()->closeTab(index, true);
        if (group->tabWidget()->count() == 0 && m_groups.size() > 1) {
            removeGroup(group);
        }
    });
}

void EditorArea::removeGroup(EditorGroup* group) {
    if (m_groups.size() <= 1) return;

    m_groups.removeOne(group);
    group->deleteLater();

    if (m_activeGroup == group) {
        setActiveGroup(m_groups.first());
    }
}

void EditorArea::setActiveGroup(EditorGroup* group) {
    if (!group || m_activeGroup == group) return;

    if (m_activeGroup) {
        m_activeGroup->setActive(false);
    }
    m_activeGroup = group;
    m_activeGroup->setActive(true);

    emit activeFileChanged(m_activeGroup->currentFilePath());
    emit activeEditorChanged(m_activeGroup->currentEditor());
}

CodeEditor* EditorArea::currentEditor() const {
    return m_activeGroup ? m_activeGroup->currentEditor() : nullptr;
}

QString EditorArea::currentFilePath() const {
    return m_activeGroup ? m_activeGroup->currentFilePath() : QString();
}

CodeEditor* EditorArea::openFile(const QString& filePath) {
    if (!m_activeGroup) return nullptr;
    return m_activeGroup->tabManager()->openFile(filePath);
}

CodeEditor* EditorArea::newUntitledTab() {
    if (!m_activeGroup) return nullptr;
    return m_activeGroup->tabManager()->newUntitledTab();
}

bool EditorArea::closeCurrentTab() {
    if (!m_activeGroup) return false;
    int idx = m_activeGroup->tabWidget()->currentIndex();
    if (idx >= 0) {
        bool closed = m_activeGroup->tabManager()->closeTab(idx, true);
        if (closed && m_activeGroup->tabWidget()->count() == 0 && m_groups.size() > 1) {
            removeGroup(m_activeGroup);
        }
        return closed;
    }
    return false;
}

void EditorArea::closeAll() {
    QVector<EditorGroup*> toClose = m_groups;
    for (EditorGroup* g : toClose) {
        g->tabManager()->closeAllTabs(true);
        if (g->tabWidget()->count() == 0 && m_groups.size() > 1) {
            removeGroup(g);
        }
    }
}

bool EditorArea::saveCurrentFile() {
    if (!m_activeGroup) return false;
    int idx = m_activeGroup->tabWidget()->currentIndex();
    if (idx >= 0) {
        return m_activeGroup->tabManager()->saveTab(idx);
    }
    return false;
}

bool EditorArea::saveAll() {
    bool ok = true;
    for (EditorGroup* g : m_groups) {
        ok &= g->tabManager()->saveAll();
    }
    return ok;
}

void EditorArea::split(Qt::Orientation orientation) {
    m_mainSplitter->setOrientation(orientation);

    EditorGroup* newGroup = new EditorGroup(this);
    addGroup(newGroup);

    QString activeFile = currentFilePath();
    if (!activeFile.isEmpty()) {
        newGroup->tabManager()->openFile(activeFile);
    } else {
        newGroup->tabManager()->newUntitledTab();
    }

    setActiveGroup(newGroup);
}

void EditorArea::setProjectRoot(const QString& root) {
    m_projectRoot = root;
    for (EditorGroup* g : m_groups) {
        g->setProjectRoot(root);
    }
}

void EditorArea::handleGroupFocused(EditorGroup* group) {
    setActiveGroup(group);
}

void EditorArea::handleGroupFileChanged(const QString& path) {
    if (qobject_cast<EditorGroup*>(sender()) == m_activeGroup) {
        emit activeFileChanged(path);
        emit activeEditorChanged(m_activeGroup->currentEditor());
    }
}

void EditorArea::handleGroupAllTabsClosed() {
    EditorGroup* group = qobject_cast<EditorGroup*>(sender());
    if (group && m_groups.size() > 1) {
        removeGroup(group);
    }
}
