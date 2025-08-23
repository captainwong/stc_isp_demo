#include <QtWidgets/QApplication>

#include "programmer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    programmer window;
    window.show();
    return app.exec();
}
