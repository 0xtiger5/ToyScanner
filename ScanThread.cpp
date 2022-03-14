#include "ScanThread.h"
#include "Scanner.h"
#include "Products.h"

class Scanner;

ScanThread::ScanThread(QWidget* parent) {
    
}

void ScanThread::inavailable(QAbstractSocket::SocketError)
{
    quint32 ip = p.first;
    quint16 port = p.second;
    QHostAddress ipaddr(ip);
    QString s = ipaddr.toString() + ":" + port + " is not available.";
    emit outputMess(s);
}

void ScanThread::available()
{
    quint32 ip = p.first;
    quint16 port = p.second;
    QHostAddress ipaddr(ip);
    QString s = ipaddr.toString() + ":" + port + " is available.";
    emit outputMess(s);
    emit addPort(p);
}

void ScanThread::run() {
    while (true)
    {
        qDebug() << tr("thread running");
        socket = new QTcpSocket;
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(inavailable(QAbstractSocket::SocketError)));
        connect(socket, SIGNAL(connected()), this, SLOT(available()));
        if (!get(Scanner::buf, p)) {
            delete socket;
            break;
        }
        quint32 ip = p.first;
        quint16 port = p.second;
        QHostAddress ipaddr(ip);
        qDebug() << "readThread is trying to connect:" << ipaddr.toString() << ":" << port;
        socket->connectToHost(ipaddr, port);
        if (socket->waitForConnected(1000))
        {
            qDebug() << "connect";
            quint32 ip = p.first;
            quint16 port = p.second;
            QHostAddress ipaddr(ip);
            QString s = ipaddr.toString() + ":" + tr("%1").arg(port) + " is available.";
            emit outputMess(s);
            emit addPort(p);
        }
        else {
            qDebug() << "not connect";
            quint32 ip = p.first;
            quint16 port = p.second;
            QHostAddress ipaddr(ip);
            QString s = ipaddr.toString() + ":" + tr("%1").arg(port) + " is not available.";
            emit outputMess(s);
        }
        delete socket;
    }
    qDebug() << "thread finish";
}

bool ScanThread::get(Products &buf, QPair<quint32, quint16> &p)
{
    buf.lock.lock();
    //判断是否结束
    if (buf.finished) {
        buf.lock.unlock();
        return false;
    }
    //判断能不能读
    while (buf.writePos == buf.readPos && !buf.finished) {
        //while可以防止虚假唤醒。
        qDebug() << "readThread is waiting...";
        buf.notEmpty.wait(&buf.lock);  //阻塞等待buf not empty，即可以读取信息。
    }
    if (buf.finished) {
        buf.lock.unlock();
        return false;
    }
    //读取信息
    p = (buf.buffer)[buf.readPos];
    qDebug() << "Thread reads: " << p.first << "_" << p.second << "\n";
    
    buf.readPos = (buf.readPos + 1) % buf.SIZE;
    buf.notFull.wakeAll();
    buf.lock.unlock();
    return true;
}