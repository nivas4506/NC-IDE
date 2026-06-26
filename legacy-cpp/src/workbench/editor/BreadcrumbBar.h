// src/workbench/editor/BreadcrumbBar.h
#pragma once
#include <QWidget>
#include <QHBoxLayout>

class BreadcrumbBar : public QWidget {
    Q_OBJECT
public:
    explicit BreadcrumbBar(QWidget* parent = nullptr);

    void setFilePath(const QString& filePath, const QString& projectRoot);

signals:
    void fileSelected(const QString& path);

private slots:
    void onSegmentClicked();

private:
    QHBoxLayout* m_layout;
    QString m_filePath;
    QString m_projectRoot;
};
