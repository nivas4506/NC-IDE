// src/palette/CommandRegistry.h
#pragma once
#include <QString>
#include <QVector>
#include <functional>

struct Command {
    QString id;
    QString label;
    QString category;
    QString shortcut;
    std::function<void()> execute;
};

class CommandRegistry {
public:
    static CommandRegistry& instance();
    
    void registerCommand(const Command& cmd);
    void registerCommand(const QString& id, const QString& label, const QString& category, 
                         const QString& shortcut, std::function<void()> execute);
                         
    QVector<Command> commands() const { return m_commands; }
    
    bool executeCommand(const QString& id);
    
private:
    CommandRegistry();
    QVector<Command> m_commands;
};
