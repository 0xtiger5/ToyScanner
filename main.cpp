#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<QVector<int>>>("QVector<QVector<int>>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<ScanThread*>("ScanThread*");
    qRegisterMetaType<QPair<quint32, quint16>>("QPair<quint32, quint16>");
    qRegisterMetaType<QVector<QPair<quint32, quint16>>>("QVector<QPair<quint32, quint16>>");
    
    MainWindow w;
    w.show();
    return a.exec();
}
