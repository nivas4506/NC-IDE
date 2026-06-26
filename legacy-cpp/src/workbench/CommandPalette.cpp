// src/palette/CommandPalette.cpp
#include "CommandPalette.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDebug>
#include <algorithm>

CommandPalette::CommandPalette(QWidget* parent)
    : QWidget(parent) {
    
    // UI layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);
    
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Type a command to execute...");
    m_searchInput->setStyleSheet(
        "background-color: #282a36; color: #f8f8f2; border: 1px solid #6272a4; border-radius: 4px; padding: 6px; font-size: 11pt;"
    );
    layout->addWidget(m_searchInput);
    
    m_listWidget = new QListWidget(this);
    m_listWidget->setStyleSheet(
        "QListWidget { background-color: #1e1e24; color: #f8f8f2; border: 1px solid #44475a; border-radius: 4px; font-size: 10pt; }"
        "QListWidget::item { padding: 6px; border-bottom: 1px solid #282a36; }"
        "QListWidget::item:selected { background-color: #44475a; color: #50fa7b; font-weight: bold; }"
    );
    layout->addWidget(m_listWidget);
    
    // Set style and dimensions
    setStyleSheet(
        "QWidget { background-color: #1e1e24; border: 1px solid #44475a; border-radius: 6px; }"
    );
    resize(500, 250);
    hide();
    
    // Event filters
    m_searchInput->installEventFilter(this);
    m_listWidget->installEventFilter(this);
    
    connect(m_searchInput, &QLineEdit::textChanged, this, &CommandPalette::onSearchTextChanged);
    connect(m_listWidget, &QListWidget::itemClicked, this, &CommandPalette::onItemClicked);
}

CommandPalette::~CommandPalette() {
}

void CommandPalette::showPalette() {
    m_allCommands = CommandRegistry::instance().commands();
    
    // Position palette at top center of parent
    if (parentWidget()) {
        int x = (parentWidget()->width() - width()) / 2;
        int y = 50; // slightly down from top menu bar
        move(x, y);
    }
    
    m_searchInput->clear();
    filterCommands("");
    show();
    raise();
    m_searchInput->setFocus();
}

void CommandPalette::hidePalette() {
    hide();
    if (parentWidget()) {
        parentWidget()->setFocus();
    }
}

void CommandPalette::onSearchTextChanged(const QString& text) {
    filterCommands(text);
}

void CommandPalette::onItemClicked(QListWidgetItem* item) {
    if (!item) return;
    QString cmdId = item->data(Qt::UserRole).toString();
    hidePalette();
    CommandRegistry::instance().executeCommand(cmdId);
}

void CommandPalette::filterCommands(const QString& query) {
    m_listWidget->clear();
    m_filteredCommands.clear();
    
    struct ScoredCommand {
        Command cmd;
        int score;
    };
    
    QVector<ScoredCommand> scoredList;
    for (const Command& cmd : m_allCommands) {
        int score = fuzzyScore(query, cmd.label, cmd.category);
        if (score >= 0) {
            scoredList.append({cmd, score});
        }
    }
    
    // Sort descending by score
    std::sort(scoredList.begin(), scoredList.end(), [](const ScoredCommand& a, const ScoredCommand& b) {
        return a.score > b.score;
    });
    
    for (const auto& sc : scoredList) {
        m_filteredCommands.append(sc.cmd);
        
        QListWidgetItem* item = new QListWidgetItem(m_listWidget);
        QString display = QString("%1: %2").arg(sc.cmd.category).arg(sc.cmd.label);
        if (!sc.cmd.shortcut.isEmpty()) {
            display += QString(" (%1)").arg(sc.cmd.shortcut);
        }
        item->setText(display);
        item->setData(Qt::UserRole, sc.cmd.id);
    }
    
    if (m_listWidget->count() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

int CommandPalette::fuzzyScore(const QString& query, const QString& label, const QString& category) {
    if (query.isEmpty()) return 1;
    
    QString q = query.toLower();
    QString fullText = (category + ": " + label).toLower();
    
    // 1. Exact prefix match
    if (fullText.startsWith(q)) {
        return 100 - fullText.indexOf(q);
    }
    // 2. Substring match
    if (fullText.contains(q)) {
        return 50 - fullText.indexOf(q);
    }
    
    // 3. Subsequence match
    int score = 0;
    int tIdx = 0;
    for (int qIdx = 0; qIdx < q.length(); ++qIdx) {
        QChar qc = q[qIdx];
        int matchIdx = fullText.indexOf(qc, tIdx);
        if (matchIdx == -1) return -1; // not a subsequence
        score += (matchIdx - tIdx);    // penalty for spacing
        tIdx = matchIdx + 1;
    }
    
    return 10 - score;
}

void CommandPalette::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hidePalette();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

bool CommandPalette::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_Escape) {
            hidePalette();
            return true;
        }
        
        if (obj == m_searchInput) {
            if (keyEvent->key() == Qt::Key_Down) {
                int nextRow = m_listWidget->currentRow() + 1;
                if (nextRow < m_listWidget->count()) {
                    m_listWidget->setCurrentRow(nextRow);
                }
                return true;
            } else if (keyEvent->key() == Qt::Key_Up) {
                int prevRow = m_listWidget->currentRow() - 1;
                if (prevRow >= 0) {
                    m_listWidget->setCurrentRow(prevRow);
                }
                return true;
            } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                QListWidgetItem* currentItem = m_listWidget->currentItem();
                onItemClicked(currentItem);
                return true;
            }
        }
    } else if (event->type() == QEvent::FocusOut) {
        // If focus shifts away, hide palette
        if (!m_searchInput->hasFocus() && !m_listWidget->hasFocus()) {
            hidePalette();
        }
    }
    
    return QWidget::eventFilter(obj, event);
}
