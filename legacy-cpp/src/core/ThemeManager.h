// src/core/ThemeManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QMap>

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();

    QStringList availableThemes() const;
    QString currentTheme() const { return m_currentTheme; }
    void setTheme(const QString& name);

    // Design token access
    QColor color(const QString& token) const;
    QString colorHex(const QString& token) const;

    // Convenience token accessors
    QColor bgPrimary() const;
    QColor bgSurface() const;
    QColor accent() const;
    QColor textPrimary() const;
    QColor textMuted() const;
    QColor border() const;
    QColor success() const;
    QColor warning() const;
    QColor error() const;

    // Full stylesheet for the application
    QString applicationStyleSheet() const;

signals:
    void themeChanged(const QString& name);

private:
    ThemeManager();
    void buildDarkTheme();
    void buildLightTheme();

    QString m_currentTheme = "dark";
    QMap<QString, QMap<QString, QString>> m_themes; // theme -> token -> hex
};
