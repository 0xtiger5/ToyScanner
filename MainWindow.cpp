#include "MainWindow.h"
#include "Products.h"

#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //textBroswer font style
    QFont font;
    font.setFamily(tr("Consolas"));
    font.setPixelSize(24);
    ui.textBrowser->setFont(font);
    
}

void MainWindow::outputMessage(QString s) {
    QString LogInfo;
    LogInfo.sprintf("%p", QThread::currentThread());
    qDebug() << "OpenSerialPort " << "threadID : " << LogInfo;
    ui.textBrowser->append(s);
}

void MainWindow::addThread(ScanThread* scanThread) {
    threadPool->start(scanThread);
}


void MainWindow::startScanning() {
    threadPool = QThreadPool::globalInstance();
    scanner = new Scanner(this);
    connect(this, &MainWindow::send, scanner, &Scanner::setInfo);
    connect(scanner, &Scanner::outputMessage, this, &MainWindow::outputMessage);
    connect(scanner, &Scanner::addThread, this, &MainWindow::addThread);
    connect(scanner, &Scanner::finished, this, &MainWindow::finishScanning);
    connect(scanner, &Scanner::appendPair, this, &MainWindow::appendPair);

    ui.textBrowser->clear();
    result.clear();
    QVector<QVector<int>> IPScope = getIPScope();
    QVector<int> portScope = getPortScope();
    int threadNum = getThreadNum();

    if (checkInfo(IPScope, portScope, threadNum)) {
        emit send(IPScope, portScope, threadNum);
        scanner->setAutoDelete(true);
        threadPool->start(scanner);
    }
}

void MainWindow::appendPair(QPair<quint32, quint16> p) {
    qDebug() << "appendPair" << p;
    result.append(p);
}

void MainWindow::finishScanning() {
    qDebug() << "finishScanning";
    QString LogInfo;
    LogInfo.sprintf("%p", QThread::currentThread());
    qDebug() << "OpenSerialPort " << "threadID : " << LogInfo;
    ui.textBrowser->append(tr("results"));
    for (auto p : result) {
        QString ip;
        quint32 temp = p.first;
        ip.append(tr("%1.%2.%3.%4").arg(temp>>24).arg((temp>>16)%(1<<8)).arg((temp >> 8) % (1 << 8)).arg((temp) % (1 << 8)));
        ui.textBrowser->append(tr("%1:%2").arg(ip).arg(p.second));
    }
    qDebug() << "resultOK";
    Scanner::buf.lock.lock();
    Scanner::buf.notEmpty.wakeAll();
    Scanner::buf.lock.unlock();
    scanner->set_exit(true);
    qDebug() << threadPool->activeThreadCount();
    QThread::sleep(6);
    qDebug() << threadPool->activeThreadCount();
    threadPool->waitForDone();
}

void MainWindow::abort() {
    ui.textBrowser->append(tr("results"));
    for (auto p : result) {
        ui.textBrowser->append(tr("%1:%2").arg(p.first).arg(p.second));
    }
    scanner->set_exit(true);
    threadPool->waitForDone();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    scanner->set_exit(true);
    threadPool->waitForDone();
    event->accept();
}

QVector<QVector<int>> MainWindow::getIPScope() {
    QVector<QVector<int>> ans(2, QVector<int>(4));
    ans[0][0] = ui.startIP_1->text().toInt();
    ans[0][1] = ui.startIP_2->text().toInt();
    ans[0][2] = ui.startIP_3->text().toInt();
    ans[0][3] = ui.startIP_4->text().toInt();

    ans[1][0] = ui.endIP_1->text().toInt();
    ans[1][1] = ui.endIP_2->text().toInt();
    ans[1][2] = ui.endIP_3->text().toInt();
    ans[1][3] = ui.endIP_4->text().toInt();
    return ans;
}

QVector<int> MainWindow::getPortScope() {
    QVector<int> ans(2);
    ans[0] = ui.startPort->text().toInt();
    ans[1] = ui.endPort->text().toInt();
    return ans;
}

int MainWindow::getThreadNum() {
    return ui.threadNum->text().toInt();
}

bool MainWindow::checkInfo(const QVector<QVector<int>> IPScope, const QVector<int> portScope, const int threadNum) {
    if (threadNum <= 0) {
        QMessageBox::warning(this, tr("Error threadNum"), tr("Invalid threadNum!"));
        return false;
    }
    unsigned int IP[2]{ 0, 0 };
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            if (IPScope[i][j] < 0 || IPScope[i][j] > 255) {
                QMessageBox::warning(this, tr("Error IP"), tr("Invalid IP Address!"));
                return false;
            }
            IP[i] += IPScope[i][j] << (8 * (3 - j));
        }
    }
    if (IP[0] > IP[1]) {
        QMessageBox::warning(this, tr("Error IP"), tr("startIP is greater than endIP!"));
        return false;
    }
    for (int i = 0; i < 2; ++i) {
        if (portScope[i] < 0 || portScope[i] > 65535) {
            QMessageBox::warning(this, tr("Error Port"), tr("Invalid Port number!"));
            return false;
        }
    }
    if (portScope[0] > portScope[1]) {
        QMessageBox::warning(this, tr("Error Port"), tr("startPort is greater than endPort!"));
        return false;
    }
    return true;
}
