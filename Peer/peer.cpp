#include "peer.h"


Peer::Peer(QObject* parent, quint16 listen_port)
    : QObject(parent),

      my_listen_port_(listen_port),
      server_connection_(nullptr),
      is_active_(false) {
  tcp_server_ = new TcpServer(this);

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

  ClientLogger::Instance()->WriteLog(LogType::INFO, " My IP: " + my_ip_.toString());
        
  connect(tcp_server_, SIGNAL(NewConnection(Connection*)), 
                 this, SLOT(SetSocket(Connection*)));
  connect(tcp_server_, SIGNAL(NewServerConnection(Connection*)), 
                 this, SLOT(OnServerConnected(Connection*)));

  connect(&update_info_timer_, SIGNAL(timeout()), 
                         this, SLOT(SendUpdateInfo()));
  connect(&update_receiver_, SIGNAL(readyRead()), 
                       this, SLOT(UpdateFriendsInfo()));
}

QHostAddress Peer::get_my_ip() const {
  return my_ip_;
}
quint16 Peer::get_my_port() const {
  return tcp_server_->serverPort();
}

void Peer::set_receiver_ip(const QHostAddress& receiver_ip) {
  receiver_ip_ = receiver_ip;
}

void Peer::set_receiver_port(const quint16& receiver_port) {
  receiver_port_ = receiver_port;
}

void Peer::set_login(const QString login) {
  my_login_ = login;
}

void Peer::set_id(const quint32 id) { my_id_ = id; }

void Peer::set_server_ip_port(QHostAddress server_ip,
                                     quint16 server_port) {
  server_ip_ = server_ip;
  server_port_ = server_port;
}

bool Peer::is_active() {
  return is_active_;
}

bool Peer::StartListening(quint16 listen_port) {
  tcp_server_->set_remote_server_ip_port(server_ip_, server_port_);
  my_listen_port_ = listen_port;
  tcp_server_->close();
  if (!tcp_server_->listen(QHostAddress::Any, my_listen_port_)) {
    ClientLogger::Instance()->WriteLog(
        LogType::ERROR,
        " cannot start on: " + QString::number(my_listen_port_));


    if (!tcp_server_->listen()) {
      is_active_ = false;
      return false;
    }

  }

  ClientLogger::Instance()->WriteLog(LogType::SUCCESS,
    " Started listening on " + QString::number(get_my_port()));

  is_active_ = true;

  update_sender_.bind(QHostAddress(QHostAddress::AnyIPv4), 0);
  update_receiver_.bind(QHostAddress::AnyIPv4, my_listen_port_, QUdpSocket::ShareAddress);
  update_info_timer_.start(3000);

  return true;
}


void Peer::SendRequest(unsigned id, QString message) {
  if (connections_.find(id) == connections_.end()) {
    connections_[id] = new Connection(this); 
    if (!ConnectToPeer(id)) connections_[id]->deleteLater();
  }

  if (connections_[id]->state() == QAbstractSocket::ConnectedState) {
    Message mes = { my_id_, message };
    connections_[id]->SendMessage(mes);
    client_dal_.AddMessageToDB(message,id, my_id_);
  }
}


bool Peer::ConnectToPeer(unsigned id) {
  //tcp_socket_ = new QTcpSocket(this);
  QPair<QString, int> ip_port = client_dal_.GetIPPort(id); // marko - change to QPair<QString, quint16>  GetIPPort(const unsigned& user_id);
  connections_[id]->connectToHost(ip_port.first, ip_port.second);
  QString logMessage = receiver_ip_.toString() + " : " + QString::number(ip_port.second);

  ClientLogger::Instance()->WriteLog(LogType::INFO,
                                     " trying connect to: " + logMessage);

  if (connections_[id]->waitForConnected(5000)) {
    ClientLogger::Instance()->WriteLog(LogType::SUCCESS,
                                       " connected to:" + logMessage);
    connections_[id]->StartConnectionTimer(30000);  // 30 sec until disconnecting	
    connect(connections_[id], SIGNAL(ConnectionTimeout()), 
                        this, SLOT(DisconncetFromPeer()));
    connect(connections_[id], SIGNAL(readyRead()), 
            connections_[id], SLOT(ReceiveRequests()));


    connect(connections_[id], SIGNAL(SendMessageToUI(QString)), 
                        this, SIGNAL(SendMessageToUI(QString)));
    return true;
  } 
  else {
    ClientLogger::Instance()->WriteLog(LogType::ERROR,
                                       " cannot connect to " + logMessage);
    //tcp_socket_->deleteLater();
    //tcp_socket_ = nullptr;
    return false;
  }
}


void Peer::DisconncetFromPeer()
{
  Connection* to_delete = static_cast<Connection*>(QObject::sender());
  unsigned id = connections_.key(to_delete);
  to_delete->disconnectFromHost();
  connect(to_delete, SIGNAL(disconnected()), 
          to_delete, SLOT(deleteLater()));
  connections_.remove(id);
  
}


void Peer::SendUpdateInfo() {
  IdPort my_id_port;
  my_id_port.id = my_id_; 
  my_id_port.port = my_listen_port_;

  QByteArray to_write = Parser::IdPort_ToByteArray(my_id_port); //pack
  to_write.append(Parser::GetUnpossibleSequence());			    //append separator

  QVector<QString> friends_ip = client_dal_.GetFriendsIP();

  for (const QString& ip_to_send : friends_ip) {
    update_sender_.writeDatagram(to_write, QHostAddress(ip_to_send), my_listen_port_);
  }

  ClientLogger::Instance()->WriteLog(LogType::INFO, " update sent");
}


void Peer::UpdateFriendsInfo() {
  QByteArray datagram;
  IdPort updated_friend_info;

  // using QUdpSocket::readDatagram (API since Qt 4)
  while (update_receiver_.hasPendingDatagrams()) {
    datagram.resize(int(update_receiver_.pendingDatagramSize()));
    QHostAddress peer_address;
    update_receiver_.readDatagram(datagram.data(), datagram.size(), &peer_address);
    updated_friend_info = Parser::ParseAsIdPort(datagram);
    if (updated_friend_info.id == my_id_)    //not to process my own requests (temporary solution)
      continue;


    if (check_timers_.find(updated_friend_info.id) == check_timers_.end()) { 
      client_dal_.SetFriendStatus(updated_friend_info.id, true);

	  QTimer* timer = new QTimer();
      timer->start(10000);
      check_timers_[updated_friend_info.id] = timer;

      connect(timer, SIGNAL(timeout()), 
               this, SLOT(SetOfflineStatus()));
    } else {
      check_timers_[updated_friend_info.id]->start(10000);  //reset timer
    }
    client_dal_.UpdateIPPort(updated_friend_info.id, peer_address.toString(), updated_friend_info.port); 

    ClientLogger::Instance()->WriteLog(
        LogType::INFO, " updated " +
                           client_dal_.GetLoginById(updated_friend_info.id) +
                           "'s info");
  }
}

void Peer::SetOfflineStatus() {
  QTimer* to_delete = static_cast<QTimer*>(QObject::sender());   //get user id that came offline
  unsigned id = check_timers_.key(to_delete);

  client_dal_.SetFriendStatus(id, false);

  ClientLogger::Instance()->WriteLog(
      LogType::INFO,
      " set " + client_dal_.GetLoginById(id) + " offline status");

  check_timers_.remove(id);
  to_delete->deleteLater();  // use deleteLater() instead of delete
}

bool Peer::LogIn(QString login, QString password) {

  server_connection_ = new Connection(this);
  
  LoginInfo info;
  info.id = client_dal_.GetIDByLogin(login);
  info.password = password;
  info.port = get_my_port();

  QString test = server_ip_.toString();

  server_connection_->connectToHost(server_ip_, server_port_);

  if (!server_connection_->waitForConnected(4000)) {
    return false;
  }
  if (!server_connection_->LoginRequest(info)) {
    return false;
  }
  return true;
}


void Peer::SetSocket(Connection* connection) {

  unsigned id = client_dal_.GetIDByIPPort(connection->peerAddress().toString(), connection->peerPort());

  connections_[id] = connection;

  connect(connection, SIGNAL(readyRead()), 
          connection, SLOT(ReceiveRequests())); // try to read line to \n when recieving data

  connect(connection, SIGNAL(SendMessageToUI(QString)),
                this, SIGNAL(SendMessageToUI(QString)));
  ClientLogger::Instance()->WriteLog(
      LogType::INFO,
      " setting socket: " +
                                      QString::number(connection->localPort()));
}


void Peer::OnServerConnected(Connection* connection) {
  server_connection_ = connection;

  connect(connection, SIGNAL(readyRead()), 
          connection, SLOT(ServerWorker())); // try to read line to \n when recieving data

}
