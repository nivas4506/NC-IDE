// src/problems/ProblemsPanel.h
#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include "../../debug/ProblemParser.h"

class ProblemsPanel : public QWidget {
    Q_OBJECT
public:
    explicit ProblemsPanel(QWidget* parent = nullptr);
    ~ProblemsPanel();
    
    void setDiagnostics(const QVector<Diagnostic>& diags);
    void addDiagnostics(const QVector<Diagnostic>& diags);
    void clearDiagnostics();
    
    int errorCount() const { return m_errorCount; }
    int warningCount() const { return m_warningCount; }
    
signals:
    void problemSelected(const QString& filePath, int line, int column);
    void countsChanged(int errors, int warnings);

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    QTreeWidget* m_treeWidget;
    QLabel* m_summaryLabel;
    int m_errorCount = 0;
    int m_warningCount = 0;
    QVector<Diagnostic> m_diagnostics;
    
    void refreshTree();
};
