#pragma once
#include <QVector>
#include <QPair>
#include <QMutex>
#include <QWaitCondition>

class Products	//存放IP_Port对的空间
{
public:
	static const size_t SIZE = 64;
	QPair<quint32, quint16> buffer[SIZE];
	bool finished = false;
	QMutex lock;
	//在writePos这里写，写完+1；在readPos这里读，读完+1。
	//先写后读，所以如果writePos==readPos，empty；如果(writePos+1)%SIZE==readPos，full
	//（实际上判满的时候，在writePos的位置本来是可以写的，但这样一来写完后，就成了writePos==readPos，即empty的情况，
	//而实际上readPos的位置是可读的，这就造成了数据的覆盖和丢失。）
	size_t readPos, writePos;
	QWaitCondition notEmpty;
	QWaitCondition notFull;

	Products()
	{
		readPos = writePos = 0;
	}
};

