// src/palette/CommandRegistry.cpp
#include "CommandRegistry.h"
#include <QDebug>

CommandRegistry& CommandRegistry::instance() {
    static CommandRegistry s_instance;
    return s_instance;
}

CommandRegistry::CommandRegistry() {
}

void CommandRegistry::registerCommand(const Command& cmd) {
    // Avoid duplicate commands
    for (int i = 0; i < m_commands.size(); ++i) {
        if (m_commands[i].id == cmd.id) {
            m_commands[i] = cmd; // overwrite
            return;
        }
    }
    m_commands.append(cmd);
}

void CommandRegistry::registerCommand(const QString& id, const QString& label, const QString& category, 
                                     const QString& shortcut, std::function<void()> execute) {
    Command cmd{id, label, category, shortcut, execute};
    registerCommand(cmd);
}

bool CommandRegistry::executeCommand(const QString& id) {
    for (const Command& cmd : m_commands) {
        if (cmd.id == id) {
            if (cmd.execute) {
                cmd.execute();
                return true;
            }
        }
    }
    return false;
}
