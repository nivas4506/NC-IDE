// src/main.cpp
#include <QApplication>
#include "core/App.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("NC IDE");
    app.setOrganizationName("NC IDE");

    App coreApp;
    coreApp.initialize();

    MainWindow window;
    window.show();

    return app.exec();
}
