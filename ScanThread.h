#pragma once

#include <QThread>
#include <QMutex>
#include <QTcpSocket>
#include <QHostAddress>
#include <QReadWriteLock>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QRunnable>

class Scanner;
class Products;

class ScanThread : public QObject, public QRunnable{
	Q_OBJECT
public:
	ScanThread(QWidget* parent = Q_NULLPTR);

public slots:
	void inavailable(QAbstractSocket::SocketError);
	void available();

signals:
	void addPort(QPair<quint32, quint16> p);
	void outputMess(QString s);

protected:
	void run();

private:
	QPair<quint32, quint16> p;
	QTcpSocket* socket;
	bool get(Products &buf, QPair<quint32, quint16>& p);
};