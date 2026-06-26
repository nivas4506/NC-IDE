// src/core/ThemeManager.cpp
#include "ThemeManager.h"
#include <QApplication>

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager() {
    buildDarkTheme();
    buildLightTheme();
}

void ThemeManager::buildDarkTheme() {
    QMap<QString, QString>& t = m_themes["dark"];
    t["bg-primary"]   = "#1e1e24";
    t["bg-surface"]   = "#282a36";
    t["bg-input"]     = "#282a36";
    t["accent"]       = "#bd93f9";
    t["text-primary"] = "#f8f8f2";
    t["text-muted"]   = "#6272a4";
    t["border"]       = "#44475a";
    t["success"]      = "#50fa7b";
    t["warning"]      = "#ffb86c";
    t["error"]        = "#ff5555";
    t["info"]         = "#8be9fd";
    t["selection"]    = "#44475a";
    t["hover"]        = "#44475a";
    t["menu-bg"]      = "#282a36";
    t["menu-hover"]   = "#44475a";
}

void ThemeManager::buildLightTheme() {
    QMap<QString, QString>& t = m_themes["light"];
    t["bg-primary"]   = "#f0f0f0";
    t["bg-surface"]   = "#ffffff";
    t["bg-input"]     = "#ffffff";
    t["accent"]       = "#1313EC";
    t["text-primary"] = "#000000";
    t["text-muted"]   = "#6a6a6a";
    t["border"]       = "#d0d0d0";
    t["success"]      = "#3fb950";
    t["warning"]      = "#d29922";
    t["error"]        = "#f85149";
    t["info"]         = "#2188ff";
    t["selection"]    = "#d0d0d0";
    t["hover"]        = "#d0d0d0";
    t["menu-bg"]      = "#f0f0f0";
    t["menu-hover"]   = "#d0d0d0";
}

QStringList ThemeManager::availableThemes() const {
    return m_themes.keys();
}

void ThemeManager::setTheme(const QString& name) {
    if (!m_themes.contains(name) || m_currentTheme == name) return;
    m_currentTheme = name;

    // Apply the global stylesheet
    if (qApp) {
        qApp->setStyleSheet(applicationStyleSheet());
    }
    emit themeChanged(name);
}

QColor ThemeManager::color(const QString& token) const {
    auto it = m_themes.find(m_currentTheme);
    if (it != m_themes.end() && it->contains(token)) {
        return QColor(it->value(token));
    }
    return QColor();
}

QString ThemeManager::colorHex(const QString& token) const {
    auto it = m_themes.find(m_currentTheme);
    if (it != m_themes.end() && it->contains(token)) {
        return it->value(token);
    }
    return "#000000";
}

QColor ThemeManager::bgPrimary() const   { return color("bg-primary"); }
QColor ThemeManager::bgSurface() const   { return color("bg-surface"); }
QColor ThemeManager::accent() const      { return color("accent"); }
QColor ThemeManager::textPrimary() const { return color("text-primary"); }
QColor ThemeManager::textMuted() const   { return color("text-muted"); }
QColor ThemeManager::border() const      { return color("border"); }
QColor ThemeManager::success() const     { return color("success"); }
QColor ThemeManager::warning() const     { return color("warning"); }
QColor ThemeManager::error() const       { return color("error"); }

QString ThemeManager::applicationStyleSheet() const {
    QString bg   = colorHex("bg-primary");
    QString surf = colorHex("bg-surface");
    QString fg   = colorHex("text-primary");
    QString brd  = colorHex("border");
    QString hov  = colorHex("hover");
    QString inp  = colorHex("bg-input");

    return QString(
        "QMainWindow { background-color: %1; color: %2; }"
        "QMenuBar { background-color: %3; color: %2; border-bottom: 1px solid %4; }"
        "QMenuBar::item:selected { background-color: %5; }"
        "QMenu { background-color: %3; color: %2; border: 1px solid %4; }"
        "QMenu::item:selected { background-color: %5; }"
        "QLineEdit { background-color: %6; color: %2; border: 1px solid %4; border-radius: 3px; padding: 3px; }"
        "QPushButton { background-color: %5; color: %2; border: 1px solid %4; border-radius: 3px; padding: 4px 8px; }"
        "QPushButton:hover { background-color: %4; }"
        "QCheckBox { color: %2; }"
        "QLabel { color: %2; }"
        "QSplitter::handle { background-color: %3; }"
        "QComboBox { background-color: %6; color: %2; border: 1px solid %4; border-radius: 3px; padding: 3px; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background-color: %3; color: %2; selection-background-color: %5; }"
        "QSpinBox { background-color: %6; color: %2; border: 1px solid %4; border-radius: 3px; }"
        "QTextEdit { background-color: %6; color: %2; border: 1px solid %4; }"
        "QPlainTextEdit { background-color: %6; color: %2; }"
        "QScrollBar:vertical { background: %1; width: 10px; }"
        "QScrollBar::handle:vertical { background: %4; border-radius: 4px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar:horizontal { background: %1; height: 10px; }"
        "QScrollBar::handle:horizontal { background: %4; border-radius: 4px; min-width: 20px; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }"
    ).arg(bg, fg, surf, brd, hov, inp);
}
