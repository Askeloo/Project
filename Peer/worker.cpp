#include "worker.h"
#include "signalredirector.h"

Worker::Worker(BlockReader* reader, unsigned user_id, unsigned my_id)
    : reader_(reader),
      my_id_(my_id),
      logger_(ClientLogger::Instance()),
      redirector_(SignalRedirector::get_instance()) {
  peer_info_.id = user_id;

  timer_.start(k_msc);

  socket_ = reader_->get_socket();
  writer_ = new BlockWriter(socket_);

  redirector_.ConnectToMessageSent(this);
  redirector_.ConnectToMessageRecieved(this);

  connect(socket_, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
  connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(OnError(QAbstractSocket::SocketError)));

  connect(reader_, SIGNAL(ReadyReadBlock()), this, SLOT(OnReadyReadBlock()));

  strategies_.insert(ClientClientRequest::MESSAGE,
                     new RecieveMessageStrategy());
}

Worker::Worker(Friend peer_info, unsigned my_id)
    : peer_info_(peer_info), 
      my_id_(my_id),
      logger_(ClientLogger::Instance()),
      redirector_(SignalRedirector::get_instance()) {
  timer_.start(k_msc);

  socket_ = new QTcpSocket();
  writer_ = new BlockWriter(socket_);
  reader_ = new BlockReader(socket_);

  redirector_.ConnectToMessageSent(this);
  redirector_.ConnectToMessageRecieved(this);

  connect(&timer_, &QTimer::timeout, socket_, &QTcpSocket::disconnectFromHost);

  connect(reader_, SIGNAL(ReadyReadBlock()), this, SLOT(OnReadyReadBlock()));

  connect(socket_, SIGNAL(connected()), this, SLOT(OnConnected()));
  connect(socket_, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
  connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(OnError(QAbstractSocket::SocketError)));

  socket_->connectToHost(peer_info_.ip, peer_info_.port);

  strategies_.insert(ClientClientRequest::MESSAGE, 
                     new RecieveMessageStrategy());
}

void Worker::DoWork() { 
  strategy_->DoWork();
}

void Worker::SetStrategy(ClientClientRequest request) {
  strategy_ = strategies_[request];
}

void Worker::set_message(QString message) { 
  message_ = message; 
}

void Worker::set_my_id(unsigned id) { 
  my_id_ = id; 
}

void Worker::SendMessage(const QString& message) {
  timer_.start(k_msc);
  MessageInfo mes = {message};
  QByteArray data = Parser::Message_ToByteArray(mes);
  client_data_.AddMessageToDB(message, peer_info_.id, my_id_);
  writer_->WriteBlock(data);
  emit MessageSent(peer_info_.id, true);
}

void Worker::OnDisconnected() {
  emit Disconnected(peer_info_.id);
}

void Worker::OnError(QAbstractSocket::SocketError) {
  emit Error(peer_info_.id);
}

void Worker::OnConnected() {
  unsigned id = peer_info_.id;
  ConnectInfo connect_info = {my_id_};
  writer_->WriteBlock(Parser::ConnectInfo_ToByteArray(connect_info));
  emit Connected(id);
}

void Worker::OnReadyReadBlock() {
  QByteArray data;
  while (reader_->HasPendingBlock()) {
    data = reader_->ReadNextBlock();
    ClientClientRequest type =
        static_cast<ClientClientRequest>(Parser::getRequestType(data));

    if (strategies_.find(type) != strategies_.end()) {
      strategy_ = strategies_[static_cast<ClientClientRequest>(type)];
      Friend info;
      strategy_->set_data(data);
      strategy_->set_peer_info(peer_info_);
      strategy_->set_my_id(my_id_);
      DoWork();
    } else {
      logger_->WriteLog(
          LogType::WARNING,
          "unknown client request recieved from" + socket_->peerName());
    }
  }
}

Worker::~Worker() {
  delete writer_;
  delete reader_; 
  socket_->deleteLater(); 
}
