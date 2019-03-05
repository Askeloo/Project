#pragma once
#include <string>
#include <QObject>
#include <QTcpSocket>

#include "../DAL/dal.h"
#include <QTcpSocket>
#include <QByteArray>
#include "../Parser&Structs/parser.h"
#include "../Parser&Structs/request_types.h"
#include"../DAL/Server/serverdb.h"

//using namespace std;

class AbstractRequest : public QObject {
	Q_OBJECT
public:
	AbstractRequest(QByteArray&, DAL &);
	AbstractRequest(QByteArray&);
	//Receives socket initalized by client or a socket initialized by server.
	//Depending from the type of request one of that sockets may be nullptr.
	//That sockets will be used to send correct responses.
	virtual bool sendResponde(QTcpSocket*) = 0;
protected:
	//every derived class will override this function in order to return
	//appropriate response according to incoming request. That response will be stored in 
	
        //string fields and will be processed but sendResponde()
	ServerDB db;
        virtual void prepareResponse() = 0;
	
	
	
	//DAL& database;
	DAL database;
	QTcpSocket init_by_server;
	//QByteArray bytarr;
signals:

public slots:
	virtual void connected();

	//void readyRead();
};