// src/workbench/statusbar/StatusBar.cpp
#include "StatusBar.h"
#include <QTimer>

StatusBar::StatusBar(QWidget* parent) : QWidget(parent) {
    setFixedHeight(22);
    setStyleSheet(
        "QWidget { background-color: #191a21; border-top: 1px solid #282a36; color: #6272a4; font-size: 8.5pt; }"
        "QLabel { padding: 0px 5px; color: #6272a4; }"
        "QPushButton { background-color: transparent; border: none; color: #f8f8f2; padding: 0px 8px; font-weight: bold; height: 22px; }"
        "QPushButton:hover { background-color: #282a36; color: #50fa7b; }"
    );

    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 0, 5, 0);
    m_layout->setSpacing(0);

    // Left elements
    m_projectLabel = new QLabel("NC IDE", this);
    m_projectLabel->setStyleSheet("font-weight: bold; color: #8be9fd;");
    
    m_branchBtn = new QPushButton("🌱 Git: --", this);
    m_branchBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_branchBtn, &QPushButton::clicked, this, &StatusBar::branchIndicatorClicked);

    m_notificationLabel = new QLabel("", this);
    m_notificationLabel->setStyleSheet("color: #ffb86c; font-style: italic;");

    m_layout->addWidget(m_projectLabel);
    m_layout->addWidget(m_branchBtn);
    m_layout->addWidget(m_notificationLabel);

    m_layout->addStretch(1);

    // Right elements
    m_langBtn = new QPushButton("PlainText", this);
    m_langBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_langBtn, &QPushButton::clicked, this, &StatusBar::languageIndicatorClicked);

    m_encodingLabel = new QLabel("UTF-8", this);
    m_cursorLabel = new QLabel("Ln 1, Col 1", this);

    m_layout->addWidget(m_langBtn);
    m_layout->addWidget(m_encodingLabel);
    m_layout->addWidget(m_cursorLabel);
}

void StatusBar::setProjectInfo(const QString& name) {
    m_projectLabel->setText("💼 " + name);
}

void StatusBar::setGitBranch(const QString& branch) {
    if (branch.isEmpty()) {
        m_branchBtn->setText("🌱 Git: --");
    } else {
        m_branchBtn->setText("🌱 " + branch);
    }
}

void StatusBar::setLanguage(const QString& lang) {
    m_langBtn->setText(lang);
}

void StatusBar::setCursorPos(const QString& pos) {
    m_cursorLabel->setText(pos);
}

void StatusBar::showNotification(const QString& text, int timeoutMs) {
    m_notificationLabel->setText("🔔 " + text);
    QTimer::singleShot(timeoutMs, this, [this]() {
        m_notificationLabel->clear();
    });
}
