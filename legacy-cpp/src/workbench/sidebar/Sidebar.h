// src/workbench/sidebar/Sidebar.h
#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QLabel>

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget* parent = nullptr);

    void addView(QWidget* view, const QString& title);
    void showView(int index);
    int currentViewIndex() const;

signals:
    void collapseRequested();

private:
    QLabel* m_titleLabel;
    QStackedWidget* m_stackedWidget;
    QStringList m_viewTitles;
};
