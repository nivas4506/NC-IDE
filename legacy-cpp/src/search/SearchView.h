// src/search/SearchView.h
#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTreeWidget>
#include "ProjectSearchEngine.h"

class SearchView : public QWidget {
    Q_OBJECT
public:
    explicit SearchView(QWidget* parent = nullptr);

    void setProjectRoot(const QString& root);

signals:
    void matchSelected(const QString& filePath, int line, int column);

private slots:
    void onSearchTriggered();
    void onReplaceAllTriggered();
    void onResultDoubleClicked(QTreeWidgetItem* item, int column);

private:
    QLineEdit* m_searchInput;
    QLineEdit* m_replaceInput;
    QLineEdit* m_fileFiltersInput; // e.g. *.cpp, *.py
    
    QCheckBox* m_matchCaseCheck;
    QCheckBox* m_wholeWordCheck;
    QCheckBox* m_regexCheck;
    
    QPushButton* m_searchBtn;
    QPushButton* m_replaceBtn;

    QTreeWidget* m_resultsTree;
    QString m_projectRoot;
    ProjectSearchEngine* m_searchEngine;

    void performReplace(const QString& filePath, const QString& searchVal, const QString& replaceVal, bool matchCase, bool wholeWord, bool isRegex);
};
