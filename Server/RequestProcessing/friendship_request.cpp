#include "friendship_request.h"

FriendshipRequest::FriendshipRequest(QByteArray &request, DAL *d, QTcpSocket *s)
    : AbstractRequest(d, s) {
  request_type_ = Parser::getRequestType(request);
  income_data_ = Parser::ParseAsFriendRequestInfo(request);
  PrepareResponse();
}
void FriendshipRequest::PrepareResponse() {
  if (request_type_ == (quint8)ClientRequest::FRIENDSHIP_ACCEPTED) {
    try {
      requester_ = database_->getClient(income_data_.id);
      new_friend_ = database_->getClient(income_data_.other_login);
      requester_.AddFriend(new_friend_);
      new_friend_.AddFriend(requester_);
      database_->UpdateClient(requester_);
      database_->UpdateClient(new_friend_);

      outcome_for_requester_.id = new_friend_.GetUserId();
      outcome_for_requester_.ip = new_friend_.GetUserIp();
      outcome_for_requester_.login = new_friend_.GetUserName();
      outcome_for_requester_.port = new_friend_.GetUserPort();

      outcome_for_new_friend_.id = requester_.GetUserId();
      outcome_for_new_friend_.ip = requester_.GetUserIp();
      outcome_for_new_friend_.login = requester_.GetUserName();
      outcome_for_new_friend_.port = requester_.GetUserPort();
      doNothing_ = false;
    } catch (UserNotFound &e) {
      doNothing_ = true;
      qDebug() << e.what();
    }

  } else if (request_type_ == (quint8)ClientRequest::FRIENDSHIP_REJECTED) {
    doNothing_ = true;
  }
}

bool FriendshipRequest::SendResponde() {
  if (!doNothing_) {
    outcome_data_ =
        Parser::NewFriendInfo_ToByteArray(outcome_for_requester_);
    Logger::LogOut(outcome_data_);
    client_socket_->write(outcome_data_);
    client_socket_->waitForBytesWritten(3000);
    client_socket_->disconnectFromHost();

    outcome_data_ = Parser::NewFriendInfo_ToByteArray(outcome_for_new_friend_);

    // sending FRIEND_UPDATE_INFO//+FriendUpdateInfo(s->c)I,-LOGIN ;
    QTcpSocket output_socket;
    output_socket.connectToHost(new_friend_.GetUserIp(),
                                new_friend_.GetUserPort());

    if (output_socket.waitForConnected(5000)) {  // check if can connect if yes -> send friend_update
      Logger::LogOut(outcome_data_);
      output_socket.write(outcome_data_);
      output_socket.waitForBytesWritten(1000);
      output_socket.disconnectFromHost();
    } else {
      // go and write  info about this request into db
      new_friend_.AddPendingNotifiacation(requester_);
      database_->UpdateClient(new_friend_);
    }
    output_socket.close();
    return true;
  }
  return false;
}
