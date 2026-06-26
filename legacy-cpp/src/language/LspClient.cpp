// src/language/LspClient.cpp
#include "LspClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QCoreApplication>

LspClient::LspClient(QObject* parent) 
    : QObject(parent), m_process(new QProcess(this)), m_contentLength(-1), m_nextRequestId(1) {
    
    connect(m_process, &QProcess::readyReadStandardOutput, this, &LspClient::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &LspClient::onReadyReadStandardError);
    connect(m_process, &QProcess::errorOccurred, this, &LspClient::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), 
            this, &LspClient::onProcessFinished);
}

LspClient::~LspClient() {
    shutdown();
}

bool LspClient::startServer(Language /*lang*/, const QString& serverPath, const QStringList& args) {
    m_process->start(serverPath, args);
    if (!m_process->waitForStarted(3000)) {
        qWarning() << "Failed to start LSP server:" << serverPath;
        return false;
    }
    
    // Initialize request would go here
    QJsonObject initReq;
    initReq["jsonrpc"] = "2.0";
    initReq["id"] = m_nextRequestId++;
    initReq["method"] = "initialize";
    
    QJsonObject params;
    params["processId"] = QCoreApplication::applicationPid();
    params["rootUri"] = QJsonValue::Null;
    params["capabilities"] = QJsonObject();
    initReq["params"] = params;
    
    sendMessage(initReq);
    return true;
}

void LspClient::shutdown() {
    if (m_process->state() == QProcess::Running) {
        QJsonObject req;
        req["jsonrpc"] = "2.0";
        req["id"] = m_nextRequestId++;
        req["method"] = "shutdown";
        sendMessage(req);
        
        QJsonObject exitNotify;
        exitNotify["jsonrpc"] = "2.0";
        exitNotify["method"] = "exit";
        sendMessage(exitNotify);
        
        m_process->waitForFinished(2000);
        if (m_process->state() == QProcess::Running) {
            m_process->kill();
        }
    }
}

void LspClient::didOpen(const QString& uri, const QString& text) {
    QJsonObject notify;
    notify["jsonrpc"] = "2.0";
    notify["method"] = "textDocument/didOpen";
    
    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    textDoc["languageId"] = "cpp"; // hardcoded for stub
    textDoc["version"] = 1;
    textDoc["text"] = text;
    
    params["textDocument"] = textDoc;
    notify["params"] = params;
    
    sendMessage(notify);
}

void LspClient::didChange(const QString& uri, const QString& text) {
    QJsonObject notify;
    notify["jsonrpc"] = "2.0";
    notify["method"] = "textDocument/didChange";
    
    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    textDoc["version"] = 2; // stub
    
    QJsonArray contentChanges;
    QJsonObject change;
    change["text"] = text;
    contentChanges.append(change);
    
    params["textDocument"] = textDoc;
    params["contentChanges"] = contentChanges;
    notify["params"] = params;
    
    sendMessage(notify);
}

void LspClient::requestCompletion(const QString& uri, int line, int character) {
    QJsonObject req;
    req["jsonrpc"] = "2.0";
    req["id"] = m_nextRequestId++;
    req["method"] = "textDocument/completion";
    
    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    params["textDocument"] = textDoc;
    
    QJsonObject position;
    position["line"] = line;
    position["character"] = character;
    params["position"] = position;
    
    req["params"] = params;
    sendMessage(req);
}

void LspClient::sendMessage(const QJsonObject& message) {
    if (m_process->state() != QProcess::Running) return;
    
    QJsonDocument doc(message);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    
    QByteArray header = QString("Content-Length: %1\r\n\r\n").arg(payload.size()).toUtf8();
    m_process->write(header);
    m_process->write(payload);
}

void LspClient::onReadyReadStandardOutput() {
    m_buffer.append(m_process->readAllStandardOutput());
    
    // Parse LSP headers
    while (true) {
        if (m_contentLength < 0) {
            int headerEnd = m_buffer.indexOf("\r\n\r\n");
            if (headerEnd < 0) break; // Not enough data for header
            
            QByteArray headerData = m_buffer.left(headerEnd);
            m_buffer.remove(0, headerEnd + 4);
            
            // Extract Content-Length
            QString headerStr(headerData);
            QStringList lines = headerStr.split("\r\n");
            for (const QString& line : lines) {
                if (line.startsWith("Content-Length: ")) {
                    m_contentLength = line.mid(16).toInt();
                    break;
                }
            }
        }
        
        if (m_contentLength >= 0 && m_buffer.size() >= m_contentLength) {
            QByteArray payload = m_buffer.left(m_contentLength);
            m_buffer.remove(0, m_contentLength);
            m_contentLength = -1;
            
            processMessage(payload);
        } else {
            break; // Wait for more data
        }
    }
}

void LspClient::processMessage(const QByteArray& payload) {
    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) return;
    
    QJsonObject msg = doc.object();
    
    if (msg.contains("method")) {
        QString method = msg["method"].toString();
        if (method == "textDocument/publishDiagnostics") {
            QJsonObject params = msg["params"].toObject();
            emit diagnosticsReceived(params["uri"].toString(), params["diagnostics"].toArray());
        }
    } else if (msg.contains("id") && msg.contains("result")) {
        QJsonObject result = msg["result"].toObject();
        if (result.contains("capabilities")) {
            // Send initialized notification
            QJsonObject initializedNotify;
            initializedNotify["jsonrpc"] = "2.0";
            initializedNotify["method"] = "initialized";
            initializedNotify["params"] = QJsonObject();
            sendMessage(initializedNotify);
        } else {
            // Handle completion result
            // emit completionsReceived(...)
        }
    }
}

void LspClient::onReadyReadStandardError() {
    // qWarning() << "LSP stderr:" << m_process->readAllStandardError();
}

void LspClient::onProcessError(QProcess::ProcessError error) {
    qWarning() << "LSP Process Error:" << error;
}

void LspClient::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "LSP Process finished" << exitCode << exitStatus;
}
