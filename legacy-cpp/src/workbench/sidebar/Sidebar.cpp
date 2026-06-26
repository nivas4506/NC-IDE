// src/workbench/sidebar/Sidebar.cpp
#include "Sidebar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

Sidebar::Sidebar(QWidget* parent) : QWidget(parent) {
    setMinimumWidth(180);
    setMaximumWidth(500);
    setStyleSheet(
        "QWidget { background-color: #21222c; border-right: 1px solid #282a36; }"
        "QLabel#SidebarTitle { color: #f8f8f2; font-weight: bold; font-size: 10pt; text-transform: uppercase; }"
        "QPushButton#CollapseBtn { background-color: transparent; border: none; color: #6272a4; font-size: 10pt; padding: 4px; }"
        "QPushButton#CollapseBtn:hover { color: #ff5555; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(8);

    // Header Row
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel("EXPLORER", headerWidget);
    m_titleLabel->setObjectName("SidebarTitle");

    QPushButton* collapseBtn = new QPushButton("◀", headerWidget);
    collapseBtn->setObjectName("CollapseBtn");
    collapseBtn->setToolTip("Collapse Side Bar");
    collapseBtn->setFocusPolicy(Qt::NoFocus);
    connect(collapseBtn, &QPushButton::clicked, this, &Sidebar::collapseRequested);

    headerLayout->addWidget(m_titleLabel, 1);
    headerLayout->addWidget(collapseBtn);
    mainLayout->addWidget(headerWidget);

    // Stacked views
    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget, 1);
}

void Sidebar::addView(QWidget* view, const QString& title) {
    m_stackedWidget->addWidget(view);
    m_viewTitles.append(title);
}

void Sidebar::showView(int index) {
    if (index >= 0 && index < m_stackedWidget->count()) {
        m_stackedWidget->setCurrentIndex(index);
        m_titleLabel->setText(m_viewTitles[index]);
    }
}

int Sidebar::currentViewIndex() const {
    return m_stackedWidget->currentIndex();
}
