// src/editor/CodeEditor.cpp
#include "CodeEditor.h"
#include "LineNumberArea.h"
#include "settings/SettingsManager.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QKeyEvent>

const QString CodeEditor::BRACKET_PAIRS = "()[]{}";

CodeEditor::CodeEditor(QWidget* parent)
    : QPlainTextEdit(parent) {
    m_lineNumberArea = new LineNumberArea(this);
    m_highlighter = new SyntaxHighlighter(document(), Language::PlainText);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::matchBrackets);

    SettingsManager& settings = SettingsManager::instance();
    
    QFont font(settings.fontFamily(), settings.fontSize());
    setFont(font);
    
    // Explicitly apply theme
    QString currentTheme = settings.theme();
    applyThemeColors(currentTheme);
    m_highlighter->applyTheme(currentTheme);

    connect(&settings, &SettingsManager::fontChanged, this, [this](const QString& family, int size) {
        QFont f(family, size);
        setFont(f);
    });
    connect(&settings, &SettingsManager::themeChanged, this, [this](const QString& theme) {
        applyThemeColors(theme);
        m_highlighter->applyTheme(theme);
    });

    updateLineNumberAreaWidth(0);
    setLineWrapMode(QPlainTextEdit::NoWrap);
}

void CodeEditor::setLanguage(Language lang) {
    m_language = lang;
    if (m_highlighter) {
        m_highlighter->setLanguage(lang);
    }
}

Language CodeEditor::language() const {
    return m_language;
}

int CodeEditor::lineNumberAreaWidth() const {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 15 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
    // Handled in matchBrackets to prevent conflict between multiple setExtraSelections calls
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(m_lineNumberArea);
    
    // Gutter background
    QColor gutterBg = m_lineHighlightColor.isValid() ? m_lineHighlightColor : palette().color(QPalette::Window);
    // Let's use a blend or make it slightly darker than base background to distinguish
    painter.fillRect(event->rect(), gutterBg);

    // Draw line border
    QColor borderCol = m_lineNumberColor.isValid() ? m_lineNumberColor : Qt::gray;
    painter.setPen(borderCol);
    painter.drawLine(event->rect().width() - 1, event->rect().top(), event->rect().width() - 1, event->rect().bottom());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    int activeBlockNumber = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            if (blockNumber == activeBlockNumber) {
                painter.setPen(m_lineNumberActiveColor.isValid() ? m_lineNumberActiveColor : palette().color(QPalette::Text));
                QFont font = painter.font();
                font.setBold(true);
                painter.setFont(font);
            } else {
                painter.setPen(m_lineNumberColor.isValid() ? m_lineNumberColor : Qt::gray);
                QFont font = painter.font();
                font.setBold(false);
                painter.setFont(font);
            }
            painter.drawText(0, top, m_lineNumberArea->width() - 5, fontMetrics().height(),
                             Qt::AlignRight | Qt::AlignVCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::matchBrackets() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 1. Current line highlight
    if (!isReadOnly() && m_lineHighlightColor.isValid()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(m_lineHighlightColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    // 2. Bracket matching highlight
    int pos = textCursor().position();
    QString docText = document()->toPlainText();
    
    int bracketPos = -1;
    QChar bracketChar;
    
    if (pos < docText.length()) {
        QChar c = docText.at(pos);
        if (BRACKET_PAIRS.contains(c)) {
            bracketChar = c;
            bracketPos = pos;
        }
    }
    if (bracketPos == -1 && pos > 0) {
        QChar c = docText.at(pos - 1);
        if (BRACKET_PAIRS.contains(c)) {
            bracketChar = c;
            bracketPos = pos - 1;
        }
    }

    if (bracketPos != -1) {
        int idx = BRACKET_PAIRS.indexOf(bracketChar);
        bool isOpen = (idx % 2 == 0);
        QChar matchChar = BRACKET_PAIRS.at(isOpen ? idx + 1 : idx - 1);
        int direction = isOpen ? 1 : -1;

        int matchPos = -1;
        int depth = 1;
        int scanPos = bracketPos + direction;

        while (scanPos >= 0 && scanPos < docText.length()) {
            QChar c = docText.at(scanPos);
            if (c == bracketChar) {
                depth++;
            } else if (c == matchChar) {
                depth--;
                if (depth == 0) {
                    matchPos = scanPos;
                    break;
                }
            }
            scanPos += direction;
        }

        QTextCharFormat format;
        if (matchPos != -1) {
            format.setBackground(QColor(0, 255, 0, 60));
            format.setFontWeight(QFont::Bold);
            
            QTextEdit::ExtraSelection s1;
            s1.format = format;
            s1.cursor = textCursor();
            s1.cursor.setPosition(bracketPos);
            s1.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            extraSelections.append(s1);

            QTextEdit::ExtraSelection s2;
            s2.format = format;
            s2.cursor = textCursor();
            s2.cursor.setPosition(matchPos);
            s2.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            extraSelections.append(s2);
        } else {
            format.setBackground(QColor(255, 0, 0, 60));
            format.setFontWeight(QFont::Bold);
            
            QTextEdit::ExtraSelection s1;
            s1.format = format;
            s1.cursor = textCursor();
            s1.cursor.setPosition(bracketPos);
            s1.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            extraSelections.append(s1);
        }
    }

    setExtraSelections(extraSelections);
    emit cursorPositionChanged2(cursorPositionLabel());
}

void CodeEditor::duplicateLine() {
    QTextCursor cursor = textCursor();
    int originalPosition = cursor.position();
    int originalAnchor = cursor.anchor();
    
    cursor.beginEditBlock();
    QTextCursor lineCursor = cursor;
    lineCursor.movePosition(QTextCursor::StartOfBlock);
    lineCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString lineText = lineCursor.selectedText();
    
    lineCursor.clearSelection();
    lineCursor.insertText("\n" + lineText);
    cursor.endEditBlock();
    
    QTextCursor restoreCursor = textCursor();
    restoreCursor.setPosition(originalAnchor);
    restoreCursor.setPosition(originalPosition, QTextCursor::KeepAnchor);
    setTextCursor(restoreCursor);
}

void CodeEditor::deleteLine() {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (!cursor.atEnd()) {
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    } else {
        cursor.movePosition(QTextCursor::StartOfBlock);
        if (cursor.position() > 0) {
            cursor.movePosition(QTextCursor::PreviousCharacter);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }
    }
    cursor.removeSelectedText();
    cursor.endEditBlock();
}

void CodeEditor::moveLineUp() {
    QTextCursor cursor = textCursor();
    int originalPosition = cursor.position();
    int originalAnchor = cursor.anchor();
    
    int blockNumber = cursor.blockNumber();
    if (blockNumber == 0) return;
    
    cursor.beginEditBlock();
    
    QTextBlock currentBlock = document()->findBlockByNumber(blockNumber);
    QString currentText = currentBlock.text();
    
    QTextBlock prevBlock = document()->findBlockByNumber(blockNumber - 1);
    QString prevText = prevBlock.text();
    
    QTextCursor swapCursor(prevBlock);
    swapCursor.movePosition(QTextCursor::StartOfBlock);
    swapCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (!swapCursor.atEnd()) swapCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    
    QTextCursor currentCursor(currentBlock);
    currentCursor.movePosition(QTextCursor::StartOfBlock);
    currentCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (!currentCursor.atEnd()) currentCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    
    swapCursor.setPosition(currentCursor.position(), QTextCursor::KeepAnchor);
    swapCursor.insertText(currentText + "\n" + prevText + (currentCursor.atEnd() && !currentBlock.next().isValid() ? "" : "\n"));
    
    cursor.endEditBlock();
    
    int offset = originalPosition - currentBlock.position();
    int anchorOffset = originalAnchor - currentBlock.position();
    QTextBlock newBlock = document()->findBlockByNumber(blockNumber - 1);
    
    QTextCursor newCursor = textCursor();
    newCursor.setPosition(newBlock.position() + anchorOffset);
    newCursor.setPosition(newBlock.position() + offset, QTextCursor::KeepAnchor);
    setTextCursor(newCursor);
}

void CodeEditor::moveLineDown() {
    QTextCursor cursor = textCursor();
    int originalPosition = cursor.position();
    int originalAnchor = cursor.anchor();
    
    int blockNumber = cursor.blockNumber();
    if (blockNumber >= blockCount() - 1) return;
    
    cursor.beginEditBlock();
    
    QTextBlock currentBlock = document()->findBlockByNumber(blockNumber);
    QString currentText = currentBlock.text();
    
    QTextBlock nextBlock = document()->findBlockByNumber(blockNumber + 1);
    QString nextText = nextBlock.text();
    
    QTextCursor swapCursor(currentBlock);
    swapCursor.movePosition(QTextCursor::StartOfBlock);
    swapCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (!swapCursor.atEnd()) swapCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    
    QTextCursor nextCursor(nextBlock);
    nextCursor.movePosition(QTextCursor::StartOfBlock);
    nextCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (!nextCursor.atEnd()) nextCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    
    swapCursor.setPosition(nextCursor.position(), QTextCursor::KeepAnchor);
    swapCursor.insertText(nextText + "\n" + currentText + (nextCursor.atEnd() && !nextBlock.next().isValid() ? "" : "\n"));
    
    cursor.endEditBlock();
    
    int offset = originalPosition - currentBlock.position();
    int anchorOffset = originalAnchor - currentBlock.position();
    QTextBlock newBlock = document()->findBlockByNumber(blockNumber + 1);
    
    QTextCursor newCursor = textCursor();
    newCursor.setPosition(newBlock.position() + anchorOffset);
    newCursor.setPosition(newBlock.position() + offset, QTextCursor::KeepAnchor);
    setTextCursor(newCursor);
}

void CodeEditor::goToLine(int lineNumber) {
    if (lineNumber < 1) lineNumber = 1;
    if (lineNumber > blockCount()) lineNumber = blockCount();
    
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1);
    setTextCursor(cursor);
    centerCursor();
}

void CodeEditor::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_D) {
        duplicateLine();
        event->accept();
        return;
    }
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_K) {
        deleteLine();
        event->accept();
        return;
    }
    if (event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_Up) {
        moveLineUp();
        event->accept();
        return;
    }
    if (event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_Down) {
        moveLineDown();
        event->accept();
        return;
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Y) {
        redo();
        event->accept();
        return;
    }
    
    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::applyThemeColors(const QString& themeName) {
    if (themeName == "light") {
        m_backgroundColor = QColor("#F5F5F5");
        m_foregroundColor = QColor("#000000");
        m_lineHighlightColor = QColor("#EAEAEA");
        m_lineNumberColor = QColor("#808080");
        m_lineNumberActiveColor = QColor("#000000");
        m_selectionColor = QColor("#ADD8E6");
    } else {
        m_backgroundColor = QColor("#121212");
        m_foregroundColor = QColor("#FFFFFF");
        m_lineHighlightColor = QColor("#1E1E1E");
        m_lineNumberColor = QColor("#A0A0A0");
        m_lineNumberActiveColor = QColor("#FFFFFF");
        m_selectionColor = QColor("#2A2A2A");
    }

    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/resources/themes/" + themeName + ".json",
        QCoreApplication::applicationDirPath() + "/themes/" + themeName + ".json",
        QDir::currentPath() + "/resources/themes/" + themeName + ".json",
        QDir::currentPath() + "/themes/" + themeName + ".json"
    };

    QFile file;
    for (const QString& path : searchPaths) {
        file.setFileName(path);
        if (file.open(QIODevice::ReadOnly)) {
            break;
        }
    }

    if (file.isOpen()) {
        QByteArray data = file.readAll();
        file.close();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (rootObj.contains("editor")) {
                QJsonObject edObj = rootObj.value("editor").toObject();
                if (edObj.contains("background")) m_backgroundColor = QColor(edObj.value("background").toString());
                if (edObj.contains("foreground")) m_foregroundColor = QColor(edObj.value("foreground").toString());
                if (edObj.contains("lineHighlight")) m_lineHighlightColor = QColor(edObj.value("lineHighlight").toString());
                if (edObj.contains("lineNumber")) m_lineNumberColor = QColor(edObj.value("lineNumber").toString());
                if (edObj.contains("lineNumberActive")) m_lineNumberActiveColor = QColor(edObj.value("lineNumberActive").toString());
                if (edObj.contains("selection")) m_selectionColor = QColor(edObj.value("selection").toString());
            }
        }
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Base, m_backgroundColor);
    pal.setColor(QPalette::Text, m_foregroundColor);
    pal.setColor(QPalette::Highlight, m_selectionColor);
    pal.setColor(QPalette::HighlightedText, themeName == "light" ? Qt::black : Qt::white);
    setPalette(pal);

    matchBrackets(); // recalculate/refresh selections
}

QString CodeEditor::cursorPositionLabel() const {
    QTextCursor cursor = textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    return QString("Ln %1, Col %2").arg(line).arg(col);
}
