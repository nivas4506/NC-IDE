// src/problems/ProblemsPanel.cpp
#include "ProblemsPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QHeaderView>

ProblemsPanel::ProblemsPanel(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);
    
    // Header label for aggregate counts
    m_summaryLabel = new QLabel("0 Errors  ·  0 Warnings", this);
    m_summaryLabel->setStyleSheet("font-weight: bold; padding: 2px; color: #8A8A8A;");
    mainLayout->addWidget(m_summaryLabel);
    
    // Tree widget
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(m_treeWidget, 1);
    
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &ProblemsPanel::onItemDoubleClicked);
}

ProblemsPanel::~ProblemsPanel() {
}

void ProblemsPanel::setDiagnostics(const QVector<Diagnostic>& diags) {
    m_diagnostics = diags;
    refreshTree();
}

void ProblemsPanel::addDiagnostics(const QVector<Diagnostic>& diags) {
    m_diagnostics.append(diags);
    refreshTree();
}

void ProblemsPanel::clearDiagnostics() {
    m_diagnostics.clear();
    refreshTree();
}

void ProblemsPanel::refreshTree() {
    m_treeWidget->clear();
    
    m_errorCount = 0;
    m_warningCount = 0;
    
    // Group diagnostics by file
    QMap<QString, QVector<Diagnostic>> fileGroups;
    for (const Diagnostic& diag : m_diagnostics) {
        fileGroups[diag.filePath].append(diag);
        if (diag.severity == "Error") m_errorCount++;
        else if (diag.severity == "Warning") m_warningCount++;
    }
    
    // Update summary label
    m_summaryLabel->setText(QString("%1 Errors  ·  %2 Warnings").arg(m_errorCount).arg(m_warningCount));
    emit countsChanged(m_errorCount, m_warningCount);
    
    // Fill Tree Widget
    for (auto it = fileGroups.begin(); it != fileGroups.end(); ++it) {
        QString filePath = it.key();
        QVector<Diagnostic> diags = it.value();
        
        QTreeWidgetItem* fileItem = new QTreeWidgetItem(m_treeWidget);
        QFileInfo fi(filePath);
        fileItem->setText(0, fi.fileName());
        fileItem->setToolTip(0, filePath);
        fileItem->setExpanded(true);
        
        // Bold the filename item
        QFont font = fileItem->font(0);
        font.setBold(true);
        fileItem->setFont(0, font);
        
        for (const Diagnostic& diag : diags) {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(fileItem);
            
            QString colText = diag.column > 0 ? QString(", Col %1").arg(diag.column) : "";
            QString text = QString("[%1] Line %2%3: %4")
                               .arg(diag.severity)
                               .arg(diag.line)
                               .arg(colText)
                               .arg(diag.message);
            
            childItem->setText(0, text);
            childItem->setData(0, Qt::UserRole, diag.filePath);
            childItem->setData(0, Qt::UserRole + 1, diag.line);
            childItem->setData(0, Qt::UserRole + 2, diag.column);
            
            // Apply design token colors
            if (diag.severity == "Error") {
                childItem->setForeground(0, QBrush(QColor("#F85149"))); // --error
            } else if (diag.severity == "Warning") {
                childItem->setForeground(0, QBrush(QColor("#D29922"))); // --warning
            } else {
                childItem->setForeground(0, QBrush(QColor("#E0E0E0"))); // --text-primary
            }
        }
    }
}

void ProblemsPanel::onItemDoubleClicked(QTreeWidgetItem* item, int /* column */) {
    if (!item) return;
    
    QVariant pathVal = item->data(0, Qt::UserRole);
    if (!pathVal.isValid()) return; // it is a top-level file item, not a diagnostic
    
    QString filePath = pathVal.toString();
    int line = item->data(0, Qt::UserRole + 1).toInt();
    int col = item->data(0, Qt::UserRole + 2).toInt();
    
    emit problemSelected(filePath, line, col);
}
