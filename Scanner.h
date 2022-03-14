#pragma once

#include <QString>
#include <QTextBrowser>
#include <QRunnable>
#include <QVector>
#include <QPair>

class ScanThread;
class Products;

class Scanner : public QObject, public QRunnable {
	Q_OBJECT

public:
	Scanner(QWidget* parent = Q_NULLPTR);
	~Scanner();
	
	static Products buf;
	void run() override;
	void put(Products &buf, QPair<quint32, quint16> p);

public slots:
	void setInfo(QVector<QVector<int>> IPScope, QVector<int> portScope, int threadNum);
	void outputmess(QString s);
	void set_exit(bool exit);

signals:
	void addThread(ScanThread* scanThread);
	void outputMessage(QString s);
	void appendPair(QPair<quint32, quint16> p);
	void finished();

private:
	QVector<ScanThread*> threads;
	QVector<QVector<int>> IPScope;
	QVector<int> portScope;
	int threadNum;
	bool exit;
};