// src/git/CommitDialog.cpp
#include "CommitDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

CommitDialog::CommitDialog(QWidget* parent)
    : QDialog(parent) {
    
    setWindowTitle("Git Commit");
    setMinimumSize(400, 200);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    layout->addWidget(new QLabel("Enter commit message:", this));
    
    m_messageEdit = new QTextEdit(this);
    layout->addWidget(m_messageEdit, 1);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal, this
    );
    
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString CommitDialog::commitMessage() const {
    return m_messageEdit->toPlainText().trimmed();
}
