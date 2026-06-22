// src/main.cpp
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("NC IDE");
    app.setOrganizationName("NC IDE");

    MainWindow window;
    window.show();

    return app.exec();
}
