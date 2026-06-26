// src/debug/RunDebugView.cpp
#include "RunDebugView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

RunDebugView::RunDebugView(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(12);

    QLabel* launchHeader = new QLabel("Launch Configuration", this);
    launchHeader->setStyleSheet("font-weight: bold; color: #8be9fd; font-size: 9.5pt; text-transform: uppercase;");
    layout->addWidget(launchHeader);

    QWidget* configRow = new QWidget(this);
    QHBoxLayout* configLayout = new QHBoxLayout(configRow);
    configLayout->setContentsMargins(0, 0, 0, 0);
    configLayout->setSpacing(6);

    m_configCombo = new QComboBox(configRow);
    m_configCombo->setStyleSheet(
        "QComboBox { background-color: #282a36; color: #f8f8f2; border: 1px solid #44475a; padding: 4px; border-radius: 3px; }"
        "QComboBox::drop-down { border: none; }"
    );
    connect(m_configCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RunDebugView::onComboChanged);

    m_editConfigBtn = new QPushButton("⚙", configRow);
    m_editConfigBtn->setToolTip("Edit launch configurations (runconfig.json)");
    m_editConfigBtn->setFixedWidth(28);
    m_editConfigBtn->setStyleSheet("background-color: #44475a; color: #f8f8f2; border: 1px solid #6272a4; padding: 4px; border-radius: 3px;");
    connect(m_editConfigBtn, &QPushButton::clicked, this, &RunDebugView::editConfigurationRequested);

    configLayout->addWidget(m_configCombo, 1);
    configLayout->addWidget(m_editConfigBtn);
    layout->addWidget(configRow);

    QLabel* controlsHeader = new QLabel("Debug Controls", this);
    controlsHeader->setStyleSheet("font-weight: bold; color: #8be9fd; font-size: 9.5pt; text-transform: uppercase; margin-top: 10px;");
    layout->addWidget(controlsHeader);

    m_runBtn = new QPushButton("▶  Start Execution (F5)", this);
    m_runBtn->setStyleSheet(
        "QPushButton { background-color: #50fa7b; color: #282a36; font-weight: bold; padding: 8px; border: none; border-radius: 4px; font-size: 10pt; }"
        "QPushButton:hover { background-color: #40c060; }"
    );
    connect(m_runBtn, &QPushButton::clicked, this, &RunDebugView::runTriggered);
    layout->addWidget(m_runBtn);

    m_buildBtn = new QPushButton("⚙  Run Build Task (Ctrl+B)", this);
    m_buildBtn->setStyleSheet(
        "QPushButton { background-color: #44475a; color: #f8f8f2; padding: 8px; border: 1px solid #6272a4; border-radius: 4px; font-size: 10pt; }"
        "QPushButton:hover { background-color: #6272a4; }"
    );
    connect(m_buildBtn, &QPushButton::clicked, this, &RunDebugView::buildTriggered);
    layout->addWidget(m_buildBtn);

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    m_stopBtn = new QPushButton("■  Stop Active Process", this);
    m_stopBtn->setStyleSheet(
        "QPushButton { background-color: #ff5555; color: #f8f8f2; padding: 8px; border: none; border-radius: 4px; font-size: 10pt; }"
        "QPushButton:hover { background-color: #ff3333; }"
    );
    connect(m_stopBtn, &QPushButton::clicked, this, &RunDebugView::stopTriggered);
    controlsLayout->addWidget(m_stopBtn);

    m_stepOverBtn = new QPushButton("Step Over", this);
    m_stepOverBtn->setToolTip("Step Over (F10)");
    connect(m_stepOverBtn, &QPushButton::clicked, this, &RunDebugView::stepOverTriggered);
    controlsLayout->addWidget(m_stepOverBtn);

    m_stepIntoBtn = new QPushButton("Step Into", this);
    m_stepIntoBtn->setToolTip("Step Into (F11)");
    connect(m_stepIntoBtn, &QPushButton::clicked, this, &RunDebugView::stepIntoTriggered);
    controlsLayout->addWidget(m_stepIntoBtn);

    m_stepOutBtn = new QPushButton("Step Out", this);
    m_stepOutBtn->setToolTip("Step Out (Shift+F11)");
    connect(m_stepOutBtn, &QPushButton::clicked, this, &RunDebugView::stepOutTriggered);
    controlsLayout->addWidget(m_stepOutBtn);

    layout->addLayout(controlsLayout);

    layout->addStretch(1);
}

void RunDebugView::setConfigurations(const QStringList& names, const QString& activeName) {
    m_configCombo->blockSignals(true);
    m_configCombo->clear();
    m_configCombo->addItems(names);
    
    int idx = names.indexOf(activeName);
    if (idx >= 0) {
        m_configCombo->setCurrentIndex(idx);
    } else if (m_configCombo->count() > 0) {
        m_configCombo->setCurrentIndex(0);
    }
    m_configCombo->blockSignals(false);
}

QString RunDebugView::activeConfiguration() const {
    return m_configCombo->currentText();
}

void RunDebugView::onComboChanged(int index) {
    emit activeConfigurationChanged(index);
}
