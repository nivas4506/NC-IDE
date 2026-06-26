// src/workbench/statusbar/StatusBar.h
#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class StatusBar : public QWidget {
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = nullptr);

    void setProjectInfo(const QString& name);
    void setGitBranch(const QString& branch);
    void setLanguage(const QString& lang);
    void setCursorPos(const QString& pos);
    void showNotification(const QString& text, int timeoutMs = 3000);

signals:
    void branchIndicatorClicked();
    void languageIndicatorClicked();

private:
    QHBoxLayout* m_layout;
    QLabel* m_projectLabel;
    QPushButton* m_branchBtn;
    QPushButton* m_langBtn;
    QLabel* m_encodingLabel;
    QLabel* m_cursorLabel;
    QLabel* m_notificationLabel;
};
