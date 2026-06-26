// src/palette/CommandPalette.h
#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include "../commands/CommandRegistry.h"

class CommandPalette : public QWidget {
    Q_OBJECT
public:
    explicit CommandPalette(QWidget* parent = nullptr);
    ~CommandPalette();
    
    void showPalette();
    void hidePalette();
    
protected:
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    
private slots:
    void onSearchTextChanged(const QString& text);
    void onItemClicked(QListWidgetItem* item);
    
private:
    QLineEdit* m_searchInput;
    QListWidget* m_listWidget;
    QVector<Command> m_allCommands;
    QVector<Command> m_filteredCommands;
    
    void filterCommands(const QString& query);
    int fuzzyScore(const QString& query, const QString& label, const QString& category);
};
