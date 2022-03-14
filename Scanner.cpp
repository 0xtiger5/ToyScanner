#include "Scanner.h"
#include "ScanThread.h"
#include "Products.h"
#include <qDebug>

Products Scanner::buf;

Scanner::Scanner(QWidget* parent) {
    threadNum = QThread::idealThreadCount();
    setAutoDelete(true);
}

Scanner::~Scanner() {
    
}

void Scanner::run() //生产者
{
    exit = false;
    qDebug() << "writeThread started.\n";
    {//显示基本信息
        QString Info;
        QString IPInfo;
        IPInfo.sprintf("startIP:%d.%d.%d.%d\nendIP:%d.%d.%d.%d\n",
            IPScope[0][0], IPScope[0][1], IPScope[0][2], IPScope[0][3],
            IPScope[1][0], IPScope[1][1], IPScope[1][2], IPScope[1][3]);
        Info.append(IPInfo);
        QString portInfo;
        portInfo.sprintf("startPort:%d, endPort:%d\n", portScope[0], portScope[1]);
        Info.append(portInfo);
        QString threadNumInfo;
        threadNumInfo.sprintf("threadNum:%d\n", threadNum);
        Info.append(threadNumInfo);
        QString start;
        start.sprintf("Start scanning...\n"
            "======================================");
        Info.append(start);
        emit outputMessage(Info);
    }
    for (int i = 0; i < threadNum; ++i) {//创建工作任务
        ScanThread* thread = new ScanThread;
        thread->setAutoDelete(true);
        threads.push_back(thread);
        connect(thread, &ScanThread::outputMess, this, &Scanner::outputmess);
        connect(thread, &ScanThread::addPort, this, &Scanner::appendPair);
        qDebug() << "create read thread: " << i << "\n";
        emit addThread(thread);
    }
    quint32 startIP = (IPScope[0][3]) + (IPScope[0][2] << 8) + (IPScope[0][1] << 16) + (IPScope[0][0] << 24);
    quint32 endIP = (IPScope[1][3]) + (IPScope[1][2] << 8) + (IPScope[1][1] << 16) + (IPScope[1][0] << 24);
    quint16 startPort = portScope[0];
    quint16 endPort = portScope[1];
    for (quint32 i = startIP; i <= endIP; ++i) {
        for (quint16 j = startPort; j <= endPort; ++j) {
            //生产IP_Port对
            put(buf, QPair<quint32, quint16>(i, j));
        }
    }
    while (buf.readPos != buf.writePos);
    QThread::sleep(3);
    Scanner::buf.finished = true;
    qDebug() << "trying to send finished signal";
    QString LogInfo;
    LogInfo.sprintf("%p", QThread::currentThread());
    qDebug() << "OpenSerialPort " << "threadID : " << LogInfo;
    emit finished();
    while (exit==false);
}

void Scanner::put(Products &buf, QPair<quint32, quint16> p)
{
    buf.lock.lock();
    //判断能不能写
    while ((buf.writePos+1)%buf.SIZE == buf.readPos) {
        //while可以防止虚假唤醒。
        qDebug() << "writeThread is waiting..\n";
        buf.notFull.wait(&buf.lock);  //阻塞等待buf not full，即可以写入信息。
    }
    //写入信息
    qDebug() << "writeThread writes: " << p.first << ":" << p.second;
    (buf.buffer)[buf.writePos] = p;
    buf.writePos = (buf.writePos + 1) % buf.SIZE;
    buf.notEmpty.wakeAll();
    buf.lock.unlock();
}

void Scanner::setInfo(QVector<QVector<int>> IPScope, QVector<int> portScope, int threadNum) {
    this->IPScope = IPScope;
    this->portScope = portScope;
    this->threadNum = threadNum;
}

void Scanner::outputmess(QString s) {
    qDebug() << "scanThread output";
    emit outputMessage(s);
}

void Scanner::set_exit(bool exit) {
    this->exit = exit;
}

//void Scanner::appendPair(QPair<quint32, quint16> p) {
//    qDebug() << "appendPair" << p;
//    result.append(p);
//}
