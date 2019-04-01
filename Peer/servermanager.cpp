#include "servermanager.h"

ServerManager::ServerManager(QTcpSocket *socket, ApplicationInfo& info) 
     :  socket_(socket), 
        app_info_(info),
        logger_(ClientLogger::Instance()){
  reader_ = new BlockReader(socket_);
  writer_ = new BlockWriter(socket_);
  connect(reader_, SIGNAL(ReadyReadBlock()), this, SLOT(OnReadyReadBlock()));

  AbstractStrategy* login = new LoginResponseStrategy();
  AbstractStrategy* reg = new RegisterResponseStrategy();
  AbstractStrategy* friend_update = new FriendUpdateStrategy();

  strategies_.insert(static_cast<quint8>(ServerRequest::LOGIN_SUCCEED), login);
  strategies_.insert(static_cast<quint8>(ServerRequest::LOGIN_FAILED), login);
  strategies_.insert(static_cast<quint8>(ServerRequest::REGISTER_SUCCEED), reg);
  strategies_.insert(static_cast<quint8>(ServerRequest::REGISTER_FAILED), reg);
  strategies_.insert(static_cast<quint8>(ServerRequest::FRIEND_UPDATE_INFO), friend_update);
}

ServerManager::~ServerManager() {}

void ServerManager::set_socket(QTcpSocket* socket) { 
  socket_ = socket;
  writer_->set_socket(socket_);
  reader_->set_socket(socket_);
}

void ServerManager::SendRequest(QByteArray data) { 
  if (socket_ == nullptr) {
    socket_ = new QTcpSocket();
  }
  logger_->WriteLog(INFO, "trying connect to server on:" +
     app_info_.remote_server_ip.toString() + "  " +QString::number(app_info_.remote_server_port));
  socket_->connectToHost(app_info_.remote_server_ip,
                         app_info_.remote_server_port);
  data_ = data;
  writer_->set_socket(socket_);
  reader_->set_socket(socket_);
  connect(socket_, SIGNAL(connected()), this, SLOT(OnConnected()));
}

void ServerManager::DoWork() { 
  strategy_->DoWork(); 
}

void ServerManager::OnReadyReadBlock() { 
  QByteArray data; 
  while (reader_->HasPendingBlock())
  {
    data = reader_->ReadNextBlock();
    quint8 type = Parser::getRequestType(data);
    strategy_ = strategies_[type];
    strategy_->set_data(data);
    DoWork();
  }
}

void ServerManager::OnConnected() { 
  writer_->WriteBlock(data_);
}
