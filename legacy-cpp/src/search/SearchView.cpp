// src/search/SearchView.cpp
#include "SearchView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QRegularExpression>
#include <QSet>

SearchView::SearchView(QWidget* parent) : QWidget(parent) {
    m_searchEngine = new ProjectSearchEngine(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(6);

    // Inputs
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search...");
    m_searchInput->setStyleSheet("background-color: #282a36; color: #f8f8f2; padding: 4px; border: 1px solid #44475a;");
    layout->addWidget(m_searchInput);

    m_replaceInput = new QLineEdit(this);
    m_replaceInput->setPlaceholderText("Replace...");
    m_replaceInput->setStyleSheet("background-color: #282a36; color: #f8f8f2; padding: 4px; border: 1px solid #44475a;");
    layout->addWidget(m_replaceInput);

    m_fileFiltersInput = new QLineEdit(this);
    m_fileFiltersInput->setPlaceholderText("files to include (e.g. *.cpp, *.h)");
    m_fileFiltersInput->setStyleSheet("background-color: #282a36; color: #6272a4; padding: 4px; border: 1px solid #44475a; font-size: 8.5pt;");
    layout->addWidget(m_fileFiltersInput);

    // Options Row
    QHBoxLayout* optionsLayout = new QHBoxLayout();
    m_matchCaseCheck = new QCheckBox("Aa", this);
    m_matchCaseCheck->setToolTip("Match Case");
    m_wholeWordCheck = new QCheckBox("ab", this);
    m_wholeWordCheck->setToolTip("Match Whole Word");
    m_regexCheck = new QCheckBox(".*", this);
    m_regexCheck->setToolTip("Use Regular Expression");

    m_searchBtn = new QPushButton("Search", this);
    m_searchBtn->setStyleSheet("background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; padding: 3px 8px;");
    
    m_replaceBtn = new QPushButton("Replace All", this);
    m_replaceBtn->setStyleSheet("background-color: #ff5555; color: #f8f8f2; border: 1px solid #ff5555; padding: 3px 8px;");

    optionsLayout->addWidget(m_matchCaseCheck);
    optionsLayout->addWidget(m_wholeWordCheck);
    optionsLayout->addWidget(m_regexCheck);
    optionsLayout->addWidget(m_searchBtn);
    optionsLayout->addWidget(m_replaceBtn);
    layout->addLayout(optionsLayout);

    // Results Tree
    m_resultsTree = new QTreeWidget(this);
    m_resultsTree->setColumnCount(1);
    m_resultsTree->setHeaderHidden(true);
    m_resultsTree->setStyleSheet(
        "QTreeWidget { background-color: #21222c; border: none; color: #f8f8f2; font-size: 9.5pt; }"
        "QTreeWidget::item { padding: 3px; }"
        "QTreeWidget::item:hover { background-color: #282a36; }"
        "QTreeWidget::item:selected { background-color: #44475a; color: #8be9fd; }"
    );
    layout->addWidget(m_resultsTree, 1);

    connect(m_searchInput, &QLineEdit::returnPressed, this, &SearchView::onSearchTriggered);
    connect(m_searchBtn, &QPushButton::clicked, this, &SearchView::onSearchTriggered);
    connect(m_replaceBtn, &QPushButton::clicked, this, &SearchView::onReplaceAllTriggered);
    connect(m_resultsTree, &QTreeWidget::itemDoubleClicked, this, &SearchView::onResultDoubleClicked);
}

void SearchView::setProjectRoot(const QString& root) {
    m_projectRoot = root;
    m_searchEngine->setProjectRoot(root);
}

void SearchView::onSearchTriggered() {
    m_resultsTree->clear();
    QString term = m_searchInput->text();
    if (term.isEmpty()) return;

    QString filterStr = m_fileFiltersInput->text().trimmed();
    if (!filterStr.isEmpty()) {
        QStringList filters = filterStr.split(',', Qt::SkipEmptyParts);
        for (QString& f : filters) f = f.trimmed();
        m_searchEngine->setFileFilters(filters);
    } else {
        m_searchEngine->setFileFilters(QStringList());
    }

    bool matchCase = m_matchCaseCheck->isChecked();
    bool wholeWord = m_wholeWordCheck->isChecked();
    
    QList<ProjectSearchResult> results = m_searchEngine->search(term, matchCase, wholeWord);
    
    QMap<QString, QTreeWidgetItem*> fileGroups;
    for (const ProjectSearchResult& res : results) {
        if (!fileGroups.contains(res.filePath)) {
            QTreeWidgetItem* parentItem = new QTreeWidgetItem(m_resultsTree);
            parentItem->setText(0, "📁 " + QFileInfo(res.filePath).fileName());
            parentItem->setToolTip(0, res.filePath);
            parentItem->setData(0, Qt::UserRole, res.filePath);
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

void SearchView::onReplaceAllTriggered() {
    QString searchVal = m_searchInput->text();
    QString replaceVal = m_replaceInput->text();
    if (searchVal.isEmpty()) return;

    if (m_resultsTree->topLevelItemCount() == 0) {
        QMessageBox::information(this, "Replace All", "No search results to replace.");
        return;
    }

    auto reply = QMessageBox::question(this, "Replace All", 
        QString("Are you sure you want to replace all occurrences of '%1' with '%2' across the workspace?").arg(searchVal).arg(replaceVal),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSet<QString> filePaths;
        for (int i = 0; i < m_resultsTree->topLevelItemCount(); ++i) {
            QTreeWidgetItem* fileItem = m_resultsTree->topLevelItem(i);
            filePaths.insert(fileItem->data(0, Qt::UserRole).toString());
        }

        bool matchCase = m_matchCaseCheck->isChecked();
        bool wholeWord = m_wholeWordCheck->isChecked();
        bool isRegex = m_regexCheck->isChecked();

        for (const QString& filePath : filePaths) {
            performReplace(filePath, searchVal, replaceVal, matchCase, wholeWord, isRegex);
        }

        QMessageBox::information(this, "Replace All", QString("Replaced occurrences in %1 files.").arg(filePaths.size()));
        onSearchTriggered();
    }
}

void SearchView::performReplace(const QString& filePath, const QString& searchVal, const QString& replaceVal, bool matchCase, bool wholeWord, bool isRegex) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QString newContent = content;
    
    if (isRegex) {
        auto caseOpt = matchCase ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption;
        QRegularExpression re(searchVal, caseOpt);
        newContent.replace(re, replaceVal);
    } else {
        auto caseSensitivity = matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
        if (wholeWord) {
            QString escaped = QRegularExpression::escape(searchVal);
            auto caseOpt = matchCase ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption;
            QRegularExpression re("\\b" + escaped + "\\b", caseOpt);
            newContent.replace(re, replaceVal);
        } else {
            newContent.replace(searchVal, replaceVal, caseSensitivity);
        }
    }

    if (newContent != content) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << newContent;
            file.close();
        }
    }
}

void SearchView::onResultDoubleClicked(QTreeWidgetItem* item, int) {
    if (!item || item->parent() == nullptr) return;
    
    QString filePath = item->data(0, Qt::UserRole).toString();
    int line = item->data(0, Qt::UserRole + 1).toInt();
    int col = item->data(0, Qt::UserRole + 2).toInt();

    emit matchSelected(filePath, line, col);
}
