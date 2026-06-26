// src/git/CommitDialog.h
#pragma once
#include <QDialog>
#include <QTextEdit>

class CommitDialog : public QDialog {
    Q_OBJECT
public:
    explicit CommitDialog(QWidget* parent = nullptr);
    
    QString commitMessage() const;
    
private:
    QTextEdit* m_messageEdit;
};
