// src/language/LspClient.h
#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
#include <QJsonObject>
#include <QByteArray>
#include "../workbench/editor/SyntaxHighlighter.h"

class LspClient : public QObject {
    Q_OBJECT
public:
    explicit LspClient(QObject* parent = nullptr);
    ~LspClient();

    bool startServer(Language lang, const QString& serverPath, const QStringList& args);
    void shutdown();

    // LSP Methods (Stubs for now)
    void didOpen(const QString& uri, const QString& text);
    void didChange(const QString& uri, const QString& text);
    void requestCompletion(const QString& uri, int line, int character);

signals:
    void diagnosticsReceived(const QString& uri, const QJsonArray& diagnostics);
    void completionsReceived(const QJsonArray& items);

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess* m_process;
    QByteArray m_buffer;
    int m_contentLength;
    int m_nextRequestId;

    void sendMessage(const QJsonObject& message);
    void processMessage(const QByteArray& payload);
};
