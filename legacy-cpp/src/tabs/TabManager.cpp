// src/tabs/TabManager.cpp
#include "TabManager.h"
#include "../settings/SettingsManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>
#include <QDir>
#include <QDebug>
#include <QTabBar>

TabManager::TabManager(QTabWidget* tabWidget, QObject* parent)
    : QObject(parent), m_tabWidget(tabWidget) {
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &TabManager::onTabCloseRequested);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &TabManager::onCurrentTabChanged);
    
    if (m_tabWidget->tabBar()) {
        connect(m_tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);
    }
}

CodeEditor* TabManager::openFile(const QString& filePath) {
    QString cleanPath = QDir::cleanPath(filePath);
    int existingIndex = indexOfFile(cleanPath);
    if (existingIndex != -1) {
        m_tabWidget->setCurrentIndex(existingIndex);
        return editorAt(existingIndex);
    }

    QFile file(cleanPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(m_tabWidget, "Error Opening File",
                              QString("Could not open file for reading:\n%1").arg(file.errorString()));
        return nullptr;
    }

    QTextStream in(&file);
    QString fileContent = in.readAll();
    file.close();

    CodeEditor* editor = new CodeEditor(m_tabWidget);
    editor->setPlainText(fileContent);
    editor->document()->setModified(false);
    
    Language lang = detectLanguageFromExtension(cleanPath);
    editor->setLanguage(lang);

    int index = m_tabWidget->addTab(editor, QFileInfo(cleanPath).fileName());
    
    EditorTab tab;
    tab.filePath = cleanPath;
    tab.displayName = QFileInfo(cleanPath).fileName();
    tab.isDirty = false;
    tab.isNewFile = false;
    m_tabData[index] = tab;

    connect(editor->document(), &QTextDocument::modificationChanged, this, &TabManager::onEditorModified);

    m_tabWidget->setCurrentIndex(index);
    updateTabTitle(index);

    SettingsManager::instance().addRecentFile(cleanPath);
    emit activeFileChanged(cleanPath);

    return editor;
}

CodeEditor* TabManager::newUntitledTab() {
    CodeEditor* editor = new CodeEditor(m_tabWidget);
    editor->document()->setModified(false);

    QString name = QString("Untitled-%1").arg(m_untitledCounter++);
    int index = m_tabWidget->addTab(editor, name);

    EditorTab tab;
    tab.filePath = "";
    tab.displayName = name;
    tab.isDirty = false;
    tab.isNewFile = true;
    m_tabData[index] = tab;

    connect(editor->document(), &QTextDocument::modificationChanged, this, &TabManager::onEditorModified);

    m_tabWidget->setCurrentIndex(index);
    updateTabTitle(index);
    
    emit activeFileChanged("");
    return editor;
}

bool TabManager::closeTab(int index, bool promptIfDirty) {
    if (index < 0 || index >= m_tabWidget->count()) return false;

    if (promptIfDirty && m_tabData[index].isDirty) {
        if (!promptSaveDiscardCancel(index)) {
            return false;
        }
    }

    QWidget* widget = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    if (widget) {
        widget->deleteLater();
    }

    m_tabData.remove(index);
    
    // Shift indices down
    QMap<int, EditorTab> newMap;
    for (auto it = m_tabData.begin(); it != m_tabData.end(); ++it) {
        int key = it.key();
        if (key > index) {
            newMap[key - 1] = it.value();
        } else {
            newMap[key] = it.value();
        }
    }
    m_tabData = newMap;

    if (m_tabWidget->count() == 0) {
        emit allTabsClosed();
        emit activeFileChanged("");
    } else {
        onCurrentTabChanged(m_tabWidget->currentIndex());
    }

    return true;
}

void TabManager::closeAllTabs(bool promptIfDirty) {
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        if (!closeTab(i, promptIfDirty)) {
            break;
        }
    }
}

void TabManager::closeOthers(int keepIndex) {
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        if (i != keepIndex) {
            closeTab(i, true);
        }
    }
}

bool TabManager::saveTab(int index) {
    if (index < 0 || index >= m_tabWidget->count()) return false;
    
    EditorTab& tab = m_tabData[index];
    if (tab.isNewFile || tab.filePath.isEmpty()) {
        return saveTabAs(index, QString());
    }

    CodeEditor* editor = editorAt(index);
    if (!editor) return false;

    QFile file(tab.filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(m_tabWidget, "Error Saving File",
                              QString("Could not open file for writing:\n%1").arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << editor->toPlainText();
    file.close();

    editor->document()->setModified(false);
    tab.isDirty = false;
    updateTabTitle(index);
    emit tabDirtyStateChanged(index, false);
    return true;
}

bool TabManager::saveTabAs(int index, const QString& newPath) {
    if (index < 0 || index >= m_tabWidget->count()) return false;
    
    QString targetPath = newPath;
    if (targetPath.isEmpty()) {
        targetPath = QFileDialog::getSaveFileName(m_tabWidget, "Save File As");
        if (targetPath.isEmpty()) return false;
    }

    targetPath = QDir::cleanPath(targetPath);
    EditorTab& tab = m_tabData[index];
    tab.filePath = targetPath;
    tab.displayName = QFileInfo(targetPath).fileName();
    tab.isNewFile = false;

    CodeEditor* editor = editorAt(index);
    if (editor) {
        editor->setLanguage(detectLanguageFromExtension(targetPath));
    }

    QFile file(targetPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(m_tabWidget, "Error Saving File",
                              QString("Could not open file for writing:\n%1").arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    if (editor) {
        out << editor->toPlainText();
        editor->document()->setModified(false);
    }
    file.close();

    tab.isDirty = false;
    updateTabTitle(index);
    emit tabDirtyStateChanged(index, false);

    SettingsManager::instance().addRecentFile(targetPath);
    emit activeFileChanged(targetPath);

    return true;
}

bool TabManager::saveAll() {
    bool success = true;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        if (m_tabData[i].isDirty) {
            if (!saveTab(i)) {
                success = false;
            }
        }
    }
    return success;
}

CodeEditor* TabManager::currentEditor() const {
    int index = m_tabWidget->currentIndex();
    return editorAt(index);
}

CodeEditor* TabManager::editorAt(int index) const {
    if (index < 0 || index >= m_tabWidget->count()) return nullptr;
    return qobject_cast<CodeEditor*>(m_tabWidget->widget(index));
}

QString TabManager::filePathAt(int index) const {
    if (m_tabData.contains(index)) {
        return m_tabData[index].filePath;
    }
    return QString();
}

int TabManager::indexOfFile(const QString& filePath) const {
    QString cleanPath = QDir::cleanPath(filePath);
    for (auto it = m_tabData.begin(); it != m_tabData.end(); ++it) {
        if (QDir::cleanPath(it.value().filePath) == cleanPath) {
            return it.key();
        }
    }
    return -1;
}

int TabManager::tabCount() const {
    return m_tabWidget->count();
}

QList<EditorTab> TabManager::allOpenTabs() const {
    QList<EditorTab> list;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        if (m_tabData.contains(i)) {
            list.append(m_tabData[i]);
        }
    }
    return list;
}

void TabManager::onTabCloseRequested(int index) {
    closeTab(index, true);
}

void TabManager::onTabMoved(int from, int to) {
    if (from == to) return;
    EditorTab movedTab = m_tabData.take(from);
    if (from < to) {
        for (int i = from; i < to; ++i) {
            m_tabData[i] = m_tabData.take(i + 1);
        }
    } else {
        for (int i = from; i > to; --i) {
            m_tabData[i] = m_tabData.take(i - 1);
        }
    }
    m_tabData[to] = movedTab;
}

void TabManager::onCurrentTabChanged(int index) {
    if (index >= 0 && index < m_tabWidget->count()) {
        emit activeFileChanged(m_tabData[index].filePath);
    } else {
        emit activeFileChanged("");
    }
}

void TabManager::onEditorModified() {
    QTextDocument* doc = qobject_cast<QTextDocument*>(sender());
    if (!doc) return;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        CodeEditor* editor = editorAt(i);
        if (editor && editor->document() == doc) {
            bool isDirty = doc->isModified();
            if (m_tabData[i].isDirty != isDirty) {
                m_tabData[i].isDirty = isDirty;
                updateTabTitle(i);
                emit tabDirtyStateChanged(i, isDirty);
            }
            break;
        }
    }
}

Language TabManager::detectLanguageFromExtension(const QString& path) const {
    QString ext = QFileInfo(path).suffix().toLower();
    if (ext == "c" || ext == "h") return Language::C;
    if (ext == "cpp" || ext == "cc" || ext == "hpp" || ext == "hh") return Language::Cpp;
    if (ext == "py") return Language::Python;
    if (ext == "java") return Language::Java;
    if (ext == "js" || ext == "jsx" || ext == "mjs") return Language::JavaScript;
    if (ext == "html" || ext == "htm") return Language::Html;
    if (ext == "css") return Language::Css;
    return Language::PlainText;
}

QIcon TabManager::dirtyIndicatorIcon() const {
    QPixmap pixmap(12, 12);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#FF5555"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(3, 3, 6, 6);
    return QIcon(pixmap);
}

void TabManager::updateTabTitle(int index) {
    if (index < 0 || index >= m_tabWidget->count()) return;
    const EditorTab& tab = m_tabData[index];
    QString title = tab.displayName;
    if (tab.isDirty) {
        m_tabWidget->setTabText(index, title + " *");
        m_tabWidget->setTabIcon(index, dirtyIndicatorIcon());
    } else {
        m_tabWidget->setTabText(index, title);
        m_tabWidget->setTabIcon(index, QIcon());
    }
}

bool TabManager::promptSaveDiscardCancel(int index) {
    const EditorTab& tab = m_tabData[index];
    QMessageBox msgBox(m_tabWidget);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Unsaved Changes");
    msgBox.setText(QString("The file \"%1\" has unsaved changes. Do you want to save them?").arg(tab.displayName));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);

    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
        return saveTab(index);
    } else if (ret == QMessageBox::Discard) {
        return true;
    } else {
        return false;
    }
}
