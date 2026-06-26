// src/core/FileManager.cpp
#include "FileManager.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

FileManager::FileManager(QObject* parent)
    : QObject(parent), m_watcher(new QFileSystemWatcher(this)) {
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &FileManager::onFileChanged);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &FileManager::onDirectoryChanged);
}

FileManager::~FileManager() {}

QString FileManager::readFile(const QString& path) const {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "FileManager: Cannot read file:" << path;
        return QString();
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    return stream.readAll();
}

bool FileManager::writeFile(const QString& path, const QString& content) {
    // Ensure parent directory exists
    QFileInfo info(path);
    QDir dir = info.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "FileManager: Cannot write file:" << path;
        return false;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << content;
    file.close();
    return true;
}

bool FileManager::fileExists(const QString& path) const {
    return QFile::exists(path);
}

bool FileManager::createDirectory(const QString& path) {
    QDir dir;
    return dir.mkpath(path);
}

bool FileManager::deleteFile(const QString& path) {
    return QFile::remove(path);
}

bool FileManager::renameFile(const QString& oldPath, const QString& newPath) {
    return QFile::rename(oldPath, newPath);
}

void FileManager::watchFile(const QString& path) {
    if (!m_watcher->files().contains(path)) {
        m_watcher->addPath(path);
    }
}

void FileManager::unwatchFile(const QString& path) {
    if (m_watcher->files().contains(path)) {
        m_watcher->removePath(path);
    }
}

void FileManager::watchDirectory(const QString& path) {
    if (!m_watcher->directories().contains(path)) {
        m_watcher->addPath(path);
    }
}

void FileManager::unwatchDirectory(const QString& path) {
    if (m_watcher->directories().contains(path)) {
        m_watcher->removePath(path);
    }
}

QString FileManager::fileName(const QString& path) {
    return QFileInfo(path).fileName();
}

QString FileManager::fileExtension(const QString& path) {
    return QFileInfo(path).suffix().toLower();
}

QString FileManager::parentDirectory(const QString& path) {
    return QFileInfo(path).absolutePath();
}

bool FileManager::isTextFile(const QString& path) {
    static const QStringList textExts = {
        "c", "cpp", "h", "hpp", "py", "java", "js", "ts", "html", "css",
        "json", "md", "txt", "xml", "yaml", "yml", "toml", "ini", "cfg",
        "sh", "bat", "ps1", "cmake", "makefile", "gitignore", "rs", "go"
    };
    QString ext = fileExtension(path);
    return textExts.contains(ext) || ext.isEmpty();
}

void FileManager::onFileChanged(const QString& path) {
    if (QFile::exists(path)) {
        emit fileChanged(path);
        // QFileSystemWatcher removes the path after change on some platforms; re-add it
        if (!m_watcher->files().contains(path)) {
            m_watcher->addPath(path);
        }
    } else {
        emit fileDeleted(path);
    }
}

void FileManager::onDirectoryChanged(const QString& path) {
    emit directoryChanged(path);
}
