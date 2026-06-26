// src/core/App.h
#pragma once
#include <QObject>

class App : public QObject {
    Q_OBJECT
public:
    explicit App(QObject* parent = nullptr);
    ~App();

    void initialize();
};
