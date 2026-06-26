// src/extensions/ExtensionsView.cpp
#include "ExtensionsView.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

ExtensionsView::ExtensionsView(ExtensionManager* extManager, QWidget* parent)
    : QWidget(parent), m_extManager(extManager) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(8);

    m_listWidget = new QListWidget(this);
    m_listWidget->setStyleSheet(
        "QListWidget { background-color: #21222c; border: none; color: #f8f8f2; font-size: 10pt; }"
        "QListWidget::item { padding: 6px; border-bottom: 1px solid #282a36; }"
        "QListWidget::item:hover { background-color: #282a36; }"
        "QListWidget::item:selected { background-color: #44475a; color: #50fa7b; }"
    );
    layout->addWidget(m_listWidget, 1);

    QWidget* detailSection = new QWidget(this);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailSection);
    detailLayout->setContentsMargins(0, 0, 0, 0);
    detailLayout->setSpacing(4);

    m_detailsLabel = new QLabel("Select an extension to see details.", this);
    m_detailsLabel->setWordWrap(true);
    m_detailsLabel->setStyleSheet("color: #6272a4; font-size: 9pt;");
    detailLayout->addWidget(m_detailsLabel);

    m_toggleBtn = new QPushButton("Enable/Disable", this);
    m_toggleBtn->setEnabled(false);
    m_toggleBtn->setStyleSheet("background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; padding: 4px; border-radius: 3px;");
    connect(m_toggleBtn, &QPushButton::clicked, this, &ExtensionsView::onToggleSelected);
    detailLayout->addWidget(m_toggleBtn);

    layout->addWidget(detailSection);

    QLabel* installHeader = new QLabel("Create/Install Mock Extension", this);
    installHeader->setStyleSheet("font-weight: bold; color: #8be9fd; margin-top: 10px; font-size: 9pt; text-transform: uppercase;");
    layout->addWidget(installHeader);

    QWidget* formWidget = new QWidget(this);
    QFormLayout* form = new QFormLayout(formWidget);
    form->setContentsMargins(0, 0, 0, 0);
    form->setSpacing(4);

    m_mockIdInput = new QLineEdit(this);
    m_mockIdInput->setPlaceholderText("e.g. debugger-cpp");
    m_mockIdInput->setStyleSheet("background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; padding: 2px;");
    form->addRow("ID:", m_mockIdInput);

    m_mockNameInput = new QLineEdit(this);
    m_mockNameInput->setPlaceholderText("e.g. C++ Debugger");
    m_mockNameInput->setStyleSheet("background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; padding: 2px;");
    form->addRow("Name:", m_mockNameInput);

    m_mockDescInput = new QLineEdit(this);
    m_mockDescInput->setPlaceholderText("e.g. GDB debugger wrapper.");
    m_mockDescInput->setStyleSheet("background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; padding: 2px;");
    form->addRow("Desc:", m_mockDescInput);

    m_mockPubInput = new QLineEdit(this);
    m_mockPubInput->setPlaceholderText("e.g. GNU Org");
    m_mockPubInput->setStyleSheet("background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; padding: 2px;");
    form->addRow("Pub:", m_mockPubInput);

    layout->addWidget(formWidget);

    m_installBtn = new QPushButton("Install Extension", this);
    m_installBtn->setStyleSheet("background-color: #50fa7b; color: #282a36; font-weight: bold; border: none; padding: 6px; border-radius: 4px;");
    connect(m_installBtn, &QPushButton::clicked, this, &ExtensionsView::onInstallMockTriggered);
    layout->addWidget(m_installBtn);

    connect(m_extManager, &ExtensionManager::extensionsChanged, this, &ExtensionsView::onRefreshList);
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &ExtensionsView::onSelectionChanged);

    onRefreshList();
}

void ExtensionsView::onRefreshList() {
    m_listWidget->clear();
    QVector<ExtensionInfo> list = m_extManager->extensions();
    for (const ExtensionInfo& info : list) {
        QString statusText = info.enabled ? "Enabled" : "Disabled";
        QListWidgetItem* item = new QListWidgetItem(QString("🧩 %1 (%2)").arg(info.name).arg(statusText), m_listWidget);
        item->setData(Qt::UserRole, info.id);
        item->setData(Qt::UserRole + 1, info.enabled);
    }
    onSelectionChanged();
}

void ExtensionsView::onSelectionChanged() {
    QListWidgetItem* item = m_listWidget->currentItem();
    if (!item) {
        m_detailsLabel->setText("Select an extension to see details.");
        m_toggleBtn->setEnabled(false);
        return;
    }

    QString id = item->data(Qt::UserRole).toString();
    bool enabled = item->data(Qt::UserRole + 1).toBool();

    QVector<ExtensionInfo> list = m_extManager->extensions();
    for (const ExtensionInfo& info : list) {
        if (info.id == id) {
            m_detailsLabel->setText(QString("<b>%1</b> v%2<br/>Publisher: %3<br/>%4")
                .arg(info.name).arg(info.version).arg(info.publisher).arg(info.description));
            break;
        }
    }

    m_toggleBtn->setEnabled(true);
    m_toggleBtn->setText(enabled ? "Disable" : "Enable");
}

void ExtensionsView::onToggleSelected() {
    QListWidgetItem* item = m_listWidget->currentItem();
    if (!item) return;

    QString id = item->data(Qt::UserRole).toString();
    bool enabled = item->data(Qt::UserRole + 1).toBool();

    m_extManager->setExtensionEnabled(id, !enabled);
}

void ExtensionsView::onInstallMockTriggered() {
    QString id = m_mockIdInput->text().trimmed();
    QString name = m_mockNameInput->text().trimmed();
    QString desc = m_mockDescInput->text().trimmed();
    QString pub = m_mockPubInput->text().trimmed();

    if (id.isEmpty() || name.isEmpty() || desc.isEmpty() || pub.isEmpty()) {
        QMessageBox::warning(this, "Fields Empty", "All extension fields must be filled!");
        return;
    }

    m_extManager->installMockExtension(id, name, desc, pub);
    
    m_mockIdInput->clear();
    m_mockNameInput->clear();
    m_mockDescInput->clear();
    m_mockPubInput->clear();

    QMessageBox::information(this, "Success", "Mock extension installed successfully!");
}
