#include <QApplication>
#include <MainWindow.h>

#include <QDebug>
#include <iostream>

int main(int argc, char* argv[])
{

    QApplication app(argc, argv);

    ImageSaver fileSaver;
    fileSaver.show();

    return app.exec();
}
