// src/settings/SettingsPanel.cpp
#include "SettingsPanel.h"
#include "SettingsManager.h"
#include "../core/ThemeManager.h"
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QFontComboBox>

SettingsPanel::SettingsPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadSettings();
}

SettingsPanel::~SettingsPanel() {}

void SettingsPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Search bar
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search settings...");
    connect(m_searchInput, &QLineEdit::textChanged, this, &SettingsPanel::onSearchChanged);
    mainLayout->addWidget(m_searchInput);

    // Scroll area for settings
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    
    m_contentWidget = new QWidget(m_scrollArea);
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(20);
    m_contentLayout->setAlignment(Qt::AlignTop);

    // --- Appearance Section ---
    QWidget* appearanceSection = createSection("Appearance");
    QVBoxLayout* appLayout = static_cast<QVBoxLayout*>(appearanceSection->layout());

    QComboBox* themeCombo = new QComboBox(this);
    themeCombo->addItems(ThemeManager::instance().availableThemes());
    themeCombo->setCurrentText(ThemeManager::instance().currentTheme());
    connect(themeCombo, &QComboBox::currentTextChanged, [](const QString& theme) {
        ThemeManager::instance().setTheme(theme);
    });
    appLayout->addWidget(createSettingRow("Theme", themeCombo, "Color theme of the IDE"));

    // --- Editor Section ---
    QWidget* editorSection = createSection("Editor");
    QVBoxLayout* edLayout = static_cast<QVBoxLayout*>(editorSection->layout());

    QFontComboBox* fontCombo = new QFontComboBox(this);
    fontCombo->setCurrentFont(QFont(SettingsManager::instance().fontFamily()));
    connect(fontCombo, &QFontComboBox::currentFontChanged, [](const QFont& font) {
        SettingsManager::instance().setFontFamily(font.family());
    });
    edLayout->addWidget(createSettingRow("Font Family", fontCombo, "Controls the font family"));

    QSpinBox* fontSizeSpin = new QSpinBox(this);
    fontSizeSpin->setRange(8, 72);
    fontSizeSpin->setValue(SettingsManager::instance().fontSize());
    connect(fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), [](int size) {
        SettingsManager::instance().setFontSize(size);
    });
    edLayout->addWidget(createSettingRow("Font Size", fontSizeSpin, "Controls the font size in pixels"));

    QCheckBox* autoSaveCheck = new QCheckBox(this);
    autoSaveCheck->setChecked(SettingsManager::instance().autoSaveEnabled());
    connect(autoSaveCheck, &QCheckBox::toggled, [](bool checked) {
        SettingsManager::instance().setAutoSaveEnabled(checked);
    });
    edLayout->addWidget(createSettingRow("Auto Save", autoSaveCheck, "Controls auto save of dirty files"));

    m_contentLayout->addWidget(appearanceSection);
    m_contentLayout->addWidget(editorSection);

    // --- AI Section ---
    QWidget* aiSection = createSection("AI Integration");
    QVBoxLayout* aiLayout = static_cast<QVBoxLayout*>(aiSection->layout());

    QLineEdit* apiKeyInput = new QLineEdit(this);
    apiKeyInput->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    apiKeyInput->setText(SettingsManager::instance().aiApiKey());
    connect(apiKeyInput, &QLineEdit::textChanged, [](const QString& text) {
        SettingsManager::instance().setAiApiKey(text);
    });
    aiLayout->addWidget(createSettingRow("Gemini API Key", apiKeyInput, "Your Google Gemini API Key for AiAssistant"));

    m_contentLayout->addWidget(aiSection);
    m_contentLayout->addStretch(1);

    m_scrollArea->setWidget(m_contentWidget);
    mainLayout->addWidget(m_scrollArea);
}

QWidget* SettingsPanel::createSection(const QString& title) {
    QWidget* section = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(section);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QLabel* titleLabel = new QLabel(title, section);
    QFont f = titleLabel->font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 2);
    titleLabel->setFont(f);
    
    layout->addWidget(titleLabel);
    return section;
}

QWidget* SettingsPanel::createSettingRow(const QString& label, QWidget* control, const QString& description) {
    QWidget* row = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(row);
    layout->setContentsMargins(10, 0, 0, 0);
    layout->setSpacing(2);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* nameLabel = new QLabel(label, row);
    hLayout->addWidget(nameLabel);
    hLayout->addStretch(1);
    
    control->setMinimumWidth(150);
    hLayout->addWidget(control);

    layout->addLayout(hLayout);

    if (!description.isEmpty()) {
        QLabel* descLabel = new QLabel(description, row);
        descLabel->setStyleSheet("color: #8A8A8A;"); // text-muted
        layout->addWidget(descLabel);
    }

    return row;
}

void SettingsPanel::loadSettings() {
    // Already populated during setupUI
}

void SettingsPanel::onSearchChanged(const QString& text) {
    // Basic filter logic: hide/show rows based on search
    // Full implementation would traverse children and check labels
}
