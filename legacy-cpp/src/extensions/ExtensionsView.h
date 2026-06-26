// src/extensions/ExtensionsView.h
#pragma once
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "ExtensionManager.h"

class ExtensionsView : public QWidget {
    Q_OBJECT
public:
    explicit ExtensionsView(ExtensionManager* extManager, QWidget* parent = nullptr);

private slots:
    void onRefreshList();
    void onToggleSelected();
    void onInstallMockTriggered();
    void onSelectionChanged();

private:
    ExtensionManager* m_extManager;
    QListWidget* m_listWidget;
    
    QPushButton* m_toggleBtn;
    QLabel* m_detailsLabel;

    QLineEdit* m_mockIdInput;
    QLineEdit* m_mockNameInput;
    QLineEdit* m_mockDescInput;
    QLineEdit* m_mockPubInput;
    QPushButton* m_installBtn;
};
