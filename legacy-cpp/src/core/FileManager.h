// src/core/FileManager.h
#pragma once
#include <QObject>
#include <QString>
#include <QFileSystemWatcher>
#include <QStringList>

class FileManager : public QObject {
    Q_OBJECT
public:
    explicit FileManager(QObject* parent = nullptr);
    ~FileManager();

    // Core file operations
    QString readFile(const QString& path) const;
    bool writeFile(const QString& path, const QString& content);
    bool fileExists(const QString& path) const;
    bool createDirectory(const QString& path);
    bool deleteFile(const QString& path);
    bool renameFile(const QString& oldPath, const QString& newPath);

    // File watching
    void watchFile(const QString& path);
    void unwatchFile(const QString& path);
    void watchDirectory(const QString& path);
    void unwatchDirectory(const QString& path);

    // Utilities
    static QString fileName(const QString& path);
    static QString fileExtension(const QString& path);
    static QString parentDirectory(const QString& path);
    static bool isTextFile(const QString& path);

signals:
    void fileChanged(const QString& path);
    void fileDeleted(const QString& path);
    void directoryChanged(const QString& path);

private slots:
    void onFileChanged(const QString& path);
    void onDirectoryChanged(const QString& path);

private:
    QFileSystemWatcher* m_watcher;
};
