#include "clientcontroller.h"

ClientController::ClientController(QObject *parent)
    : QObject(parent)
{
}

ClientController::~ClientController()
{
}

void ClientController::SendMessage(PeerInfo peer_info, QString message) {
  //Message mes = {peer_info.id, message};
  //work_manager_->SetWork(static_cast<quint8>(ClientRequest::MESSAGE), peer_info,
  //                       Parser::Message_ToByteArray(mes));
}

void ClientController::LogIn(QString login, QString password)
{
}

void ClientController::Register()
{

}

void ClientController::AddFriend()
{
}

void ClientController::DeleteFriend()
{
}

void ClientController::OnFriendRequestRecieved()
{
}

void ClientController::Start()
{
}

void ClientController::Stop()
{
}
