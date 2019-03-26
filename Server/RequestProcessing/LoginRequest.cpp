#include "LoginRequest.h"

// need to add  notifiying loginned user
LoginRequest::LoginRequest(QByteArray& A, DAL* d, QTcpSocket* s)
    : AbstractRequest(d, s) {
  incoming_structure_ = Parser::ParseAsLoginInfo(A);
  Logger::LogOut(client_socket_->socketDescriptor(),A);
  PrepareResponse();
}

LoginRequest::~LoginRequest()
{
  clock_t end = clock();
  double seconds = (double)(end - start) / CLOCKS_PER_SEC;
  qDebug() << "Login request time execution is : "<<seconds << "\n";
}

void LoginRequest::PrepareResponse() {
  // check user in db  and set t or f
  try {
    requester_ = database_->getClient(incoming_structure_.id);
    if (QString::compare(requester_.GetUserPassword(),
                         incoming_structure_.password) == 0) {
      // pasword are correct
      requester_.SetUserIp(client_socket_->peerAddress());
      requester_.SetUserPort(incoming_structure_.port);
      database_->UpdateClient(requester_);  // updating ip::port
      response_to_requester_ = (quint8)ServerRequests::LOGIN_SUCCEED;
    } else {
      // incorrect password
      response_to_requester_ = (quint8)ServerRequests::LOGIN_FAILED;
    }
  } catch (UserNotFound& e) {
    response_to_requester_ = (quint8)ServerRequests::LOGIN_FAILED;
    qDebug() << e.what();
  }
}

bool LoginRequest::SendResponde() {
  if (response_to_requester_ == (quint8)ServerRequests::LOGIN_SUCCEED) {
    QByteArray b =
        Parser::Empty_ToByteArray((quint8)ServerRequests::LOGIN_SUCCEED);
    b.append(Parser::GetUnpossibleSequence());
    Logger::LogOut(client_socket_->socketDescriptor(),b);
    client_socket_->write(b);
    client_socket_->waitForBytesWritten(3000);
    SendingPendingFriendRequests();
    SendingPendingNotifications();
    database_->UpdateClient(requester_);
    client_socket_->disconnectFromHost();

    // sending FriendUpdateInfo to all friends
    requester_ = database_->getClient(requester_.GetUserName());
    send_to_friends_.id = requester_.GetUserId();
    send_to_friends_.ip = requester_.GetUserIp();
    send_to_friends_.port = requester_.GetUserPort();
    QByteArray raw_data = Parser::FriendUpdateInfo_ToByteArray(send_to_friends_);
    raw_data.append(Parser::GetUnpossibleSequence());
    QVector<unsigned int> currentFriends = requester_.GetFriends();
    QTcpSocket output_socket;  // for friends
    for (unsigned i = 0; i < currentFriends.size(); i++) {
      SendToFriend(output_socket, raw_data, currentFriends[i]);
      // zrobiti kilkist potokiv vidpovidno do kilkosti druziv
    }   

  } else {
    // sending ServerRequests::LOGIN_FAILED
    QByteArray b = Parser::Empty_ToByteArray(response_to_requester_);
    b.append(Parser::GetUnpossibleSequence());
    Logger::LogOut(client_socket_->socketDescriptor(),b);
    client_socket_->write(b);
    client_socket_->waitForBytesWritten(3000);
    // client_socket_->disconnectFromHost();
  }
  return true;
}

bool LoginRequest::SendToFriend(QTcpSocket& output_socket, QByteArray raw_data,
                                unsigned int id) {
  Client tempClient = database_->getClient(id);
  output_socket.connectToHost(tempClient.GetUserIp(), tempClient.GetUserPort());
  if (output_socket.waitForConnected(5000)) { // default is 5000 but this thing is makes bigger time of login req execution 
    Logger::LogOut(output_socket.socketDescriptor(),raw_data);                       // if 5000 exec time is around 5 sec // if 3 000 exec time is 3 sec
    output_socket.write(raw_data);
    output_socket.waitForBytesWritten(1000);
    output_socket.disconnectFromHost();
  }

  return false;
}

void LoginRequest::SendingPendingFriendRequests() {
  // this function should send to requester pending requests and pending
  // notifications
  // QTcpSocket notifiying_requester;
  QVector<uint> pendsreqs =
      requester_.Get_Pending_Requests();  // theese guys want to be friends
  QByteArray raw_data;

  for (int i = 0; i < pendsreqs.size(); i++) {
    AddFriendInfo possible_friend;
    Client tmp = database_->getClient(pendsreqs[i]);
    possible_friend.requester_login = tmp.GetUserName();
    possible_friend.requester_id = tmp.GetUserId();
    raw_data = Parser::AddFriendInfo_ToByteArray(possible_friend);
    raw_data.append(Parser::GetUnpossibleSequence());
    Logger::LogOut(client_socket_->socketDescriptor(),raw_data);
    client_socket_->write(raw_data);
    client_socket_->waitForBytesWritten(1000);
    requester_.RemovePendingFriendRequest(tmp);
  }
  
  
}

void LoginRequest::SendingPendingNotifications() {
  // fetch data about that users, construct
  // FRIEND_UPDATE_INFO//+FriendUpdateInfo out of them and send it to currently
  // connected client
  QVector<uint> pend_notif = requester_.Get_Pending_Notifications();
  QByteArray raw_data;

  for (int i = 0; i < pend_notif.size(); i++) {
    NewFriendInfo from_new_friend;
    Client tmp = database_->getClient(pend_notif[i]);
    from_new_friend.id = tmp.GetUserId();
    from_new_friend.ip = tmp.GetUserIp();
    from_new_friend.login = tmp.GetUserName();
    from_new_friend.port = tmp.GetUserPort();
    raw_data = Parser::NewFriendInfo_ToByteArray(from_new_friend);
    raw_data.append(Parser::GetUnpossibleSequence());
    Logger::LogOut(client_socket_->socketDescriptor(),raw_data);
    client_socket_->write(raw_data);
    client_socket_->waitForBytesWritten(1000);
    requester_.RemovePendingNotification(tmp);
  }
}
