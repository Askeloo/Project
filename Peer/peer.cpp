#include "peer.h"


Peer::Peer(QObject *parent, quint16 listen_port)
        : QObject(parent)
        , tcp_server_(new TcpServer(this))
        , connection_(new Connection(this))
		, my_listen_port_(listen_port)
{
	is_active_ = true;
	if (!tcp_server_->listen(QHostAddress::Any, my_listen_port_))
	{
		qDebug() << "cannot start on: " + QString::number(my_listen_port_);
		if (!tcp_server_->listen())
		{
			is_active_ = false;
			return;
		}
	}
	qDebug() << "started listening on: " + QString::number(my_listen_port_);

		ClientDAL::ClientDB db;
	//db.AddNewFriend("admin", 1);

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    
	// use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            my_ip_ = ipAddressesList.at(i);
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (my_ip_.toString().isEmpty())
        my_ip_ = QHostAddress(QHostAddress::LocalHost);


	//BD

	//ClientDB testBD;

    //in_.setDevice(tcp_socket_);

	emit SendLog("My IP: " + my_ip_.toString());

    connect(tcp_server_, SIGNAL(NewConnection(Connection*)), this, SLOT(SetSocket(Connection*))); // 
	//connect(receiver_socket_, SIGNAL(error()),this, SLOT(DisplayError())); //for future errors

    //cannot catch this signal
}

bool Peer::is_active()
{
	return is_active_;
}


void Peer::SendRequest(unsigned id, QString message)
{
	if (connections_.find(id) == connections_.end())
	{
		connections_[id] = new Connection(this);
		ConnectToPeer(id);
	}
	
	if (connections_[id]->state() == QAbstractSocket::ConnectedState)
	{
		connections_[id]->SendMessage(message);
	}
}

//id zzz
bool Peer::ConnectToPeer(unsigned id)
{
	//tcp_socket_ = new QTcpSocket(this);
	ClientDAL::ClientDB cdb;
	QPair<QString, int> ip_port = cdb.GetIPPort(id); // marko - change to QPair<QString, quint16>  GetIPPort(const unsigned& user_id);
	connections_[id]->connectToHost(ip_port.first, ip_port.second);
	QString logMessage = receiver_ip_.toString() + " : " + QString::number(ip_port.second);
	emit SendLog("trying connect to: " + logMessage);
	if (connections_[id]->waitForConnected(5000))
	{
		emit SendLog("connected to:" + logMessage);
		//connect(tcp_socket_, SIGNAL(disconnected()), tcp_socket_, SLOT(deleteLater())); //woking with one socket so temporary commnent
		//connect(tcp_socket_, SIGNAL(destroyed()), this, SLOT(nullTcpSocket()));    
		//Connection* temp = connections_[2];
		connect(connections_[id], SIGNAL(readyRead()), connections_[id], SLOT(TryReadLine()));
		connect(connections_[id], SIGNAL(SendLog(QString)), this, SIGNAL(SendLog(QString)));
		connect(connections_[id], SIGNAL(SendMessageToUI(QString)), this, SIGNAL(SendMessageToUI(QString)));
		return true;
	}
	else
	{
		emit SendLog("cannot connect to " + logMessage);
		//tcp_socket_->deleteLater();
		//tcp_socket_ = nullptr;
		return false;
	}
}

void Peer::SetSocket(Connection *connection)
{
	ClientDAL::ClientDB cdb;
	unsigned id = cdb.GetIDByIpPort(connection->peerAddress().toString(), connection->peerPort());
	if (id == 0)	
	{
		connections_[id] = connection;

		connect(connection, SIGNAL(readyRead()), connection, SLOT(TryReadLine())); // try to read line to \n when recieving data
		connect(connection, SIGNAL(SendLog(QString)), this, SIGNAL(SendLog(QString)));
		connect(connection, SIGNAL(SendMessageToUI(QString)), this, SIGNAL(SendMessageToUI(QString)));
	}

//	emit SendLog("setting socket: ");
		//don`t need old socket if having new connection

	emit SendLog("setting socket: " + QString::number(connection_->localPort()));
	//connect(connection_, SIGNAL(readyRead()), connection_, SLOT(TryReadLine())); // try to read line to \n when recieving data
	//connect(connection_, SIGNAL(SendLog(QString)), this, SIGNAL(SendLog(QString)));
	//connect(connection_, SIGNAL(SendMessageToUI(QString)), this, SIGNAL(SendMessageToUI(QString))); to  return 

	
}

void Peer::nullTcpSocket()
{
	connection_ = nullptr;
}

void Peer::ReadMessage() //not working should, look at TryReadLine()
{

	//qDebug() << "kek";
	//in_.startTransaction();

	//in_ >> received_message_;
	//
	//if (!in_.commitTransaction())
	//{
	//	return;
	//}
	//else
	//{
	//	QString str = tr("<%1>: %2").arg(receiver_socket_->peerAddress().toString()).arg(receiver_socket_);
	//	emit SendMessageToUI(str);

	//	receiver_socket_->disconnectFromHost();
	//}

}

void Peer::DisplayError(QAbstractSocket::SocketError socketError)
{
	/*
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information(this, tr("Fortune Client"),
								 tr("The host was not found. Please check the "
									"host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr("Fortune Client"),
								 tr("The connection was refused by the peer. "
									"Make sure the fortune server is running, "
									"and check that the host name and port "
									"settings are correct."));
		break;
	default:
		QMessageBox::information(this, tr("Fortune Client"),
								 tr("The following error occurred: %1.")
								 .arg(m_sender_socket->errorString()));
	}
	*/
}
