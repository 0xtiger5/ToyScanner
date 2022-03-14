#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QHostAddress>
#include <QCloseEvent>
#include <QThreadPool>
#include "ui_Scanner.h"
#include "ScanThread.h"
#include "Scanner.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow() { if(!scanner) delete scanner; }
public slots:
    void outputMessage(QString s);
    void addThread(ScanThread* scanThread);
    void appendPair(QPair<quint32, quint16> p);
    void startScanning();
    void finishScanning();
    void abort();

signals:
    void send(QVector<QVector<int>>, QVector<int>, int);
    

private:
    Ui::ScannerClass ui;
    QThreadPool* threadPool;
    Scanner* scanner;
    QVector<QPair<quint32, quint16>> result;

    void closeEvent(QCloseEvent* event);
    QVector<QVector<int>> getIPScope();
    QVector<int> getPortScope();
    int getThreadNum();
    bool checkInfo(const QVector<QVector<int>> IPScope, const QVector<int> portScope, const int threadNum);
};