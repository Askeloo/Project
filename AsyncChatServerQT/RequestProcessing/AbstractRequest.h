#pragma once
#include <string>
#include <QObject>
#include <QTcpSocket>

#include "../DAL/dal.h"
#include <QTcpSocket>
#include <QByteArray>


//using namespace std;

class AbstractRequest : public QObject {
	Q_OBJECT
public:
	AbstractRequest(QByteArray&, DAL &);

	//Receives socket initalized by client or a socket initialized by server.
	//Depending from the type of request one of that sockets may be nullptr.
	//That sockets will be used to send correct responses.
	virtual bool sendResponde(QTcpSocket*) = 0;
protected:
	//every derived class will override this function in order to return
	//appropriate response according to incoming request. That response will be stored in 
	//string fields and will be processed but sendResponde()
	virtual void prepareResponse() = 0;
	
	//handel to acccess databse
	DAL& database;
	QTcpSocket init_by_server;

signals:

public slots:
	virtual void connected();
	

	//void readyRead();
};