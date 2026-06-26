// src/workbench/activitybar/ActivityBar.h
#pragma once
#include <QWidget>
#include <QButtonGroup>
#include <QToolButton>

class ActivityBar : public QWidget {
    Q_OBJECT
public:
    explicit ActivityBar(QWidget* parent = nullptr);

    int activeIndex() const { return m_activeIndex; }
    void setActiveIndex(int index);

signals:
    void activityChanged(int index);
    void toggleSidebar();

private slots:
    void onButtonClicked(int id);

private:
    QButtonGroup* m_buttonGroup;
    int m_activeIndex = 0;
};
