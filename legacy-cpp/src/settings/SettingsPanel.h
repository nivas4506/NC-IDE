// src/settings/SettingsPanel.h
#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>

class SettingsPanel : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPanel(QWidget* parent = nullptr);
    ~SettingsPanel();

private slots:
    void onSearchChanged(const QString& text);

private:
    QLineEdit* m_searchInput;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QVBoxLayout* m_contentLayout;

    void setupUI();
    QWidget* createSection(const QString& title);
    QWidget* createSettingRow(const QString& label, QWidget* control, const QString& description);
    
    void loadSettings();
};
