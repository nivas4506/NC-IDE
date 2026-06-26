// src/workbench/activitybar/ActivityBar.cpp
#include "ActivityBar.h"
#include <QVBoxLayout>

ActivityBar::ActivityBar(QWidget* parent) : QWidget(parent) {
    setFixedWidth(50);
    setStyleSheet(
        "QWidget { background-color: #191a21; border-right: 1px solid #282a36; }"
        "QToolButton { background-color: transparent; border: none; border-left: 3px solid transparent; color: #6272a4; font-size: 16pt; padding: 10px 0px; width: 50px; height: 50px; }"
        "QToolButton:hover { color: #f8f8f2; background-color: #282a36; }"
        "QToolButton:checked { color: #50fa7b; border-left: 3px solid #50fa7b; background-color: #21222c; }"
    );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 5);
    layout->setSpacing(5);

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);

    QStringList icons = {"📂", "🔍", "🌱", "▶", "🧩"};
    QStringList tooltips = {"Explorer (Ctrl+Shift+E)", "Search (Ctrl+Shift+F)", "Source Control (Ctrl+Shift+G)", "Run & Debug (Ctrl+Shift+D)", "Extensions (Ctrl+Shift+X)"};

    for (int i = 0; i < icons.size(); ++i) {
        QToolButton* btn = new QToolButton(this);
        btn->setText(icons[i]);
        btn->setCheckable(true);
        btn->setToolTip(tooltips[i]);
        btn->setFocusPolicy(Qt::NoFocus);
        
        layout->addWidget(btn);
        m_buttonGroup->addButton(btn, i);
    }

    layout->addStretch(1); // Push buttons to top

    connect(m_buttonGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, &ActivityBar::onButtonClicked);

    // Initial default state
    if (auto* firstBtn = m_buttonGroup->button(0)) {
        firstBtn->setChecked(true);
    }
}

void ActivityBar::onButtonClicked(int id) {
    if (id == m_activeIndex) {
        emit toggleSidebar();
    } else {
        m_activeIndex = id;
        emit activityChanged(id);
    }
}

void ActivityBar::setActiveIndex(int index) {
    if (index >= 0 && index < 5) {
        m_activeIndex = index;
        if (auto* btn = m_buttonGroup->button(index)) {
            btn->blockSignals(true);
            btn->setChecked(true);
            btn->blockSignals(false);
        }
    }
}
