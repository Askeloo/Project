#include "clientworker.h"

ClientWorker::ClientWorker(QObject* parent) : QObject(parent) {}

ClientWorker::~ClientWorker() {}

void ClientWorker::SendRequest(PeerInfo peer_info, quint8 request_type,
                               QByteArray data) {
  quint32 id = peer_info.id;

  AbstractStrategy* strategy;

  switch (request_type) {
    case static_cast<quint8>(ClientClientRequest::MESSAGE): {
      //strategy = new MessageStrategy(nullptr, peer_info, data,
      //                               connections_[id]);
      break;
    }
    default:{ 
      throw 1;
    }
  }
//  strategy->DoWork();
  delete strategy;
}

void ClientWorker::OnRequestRecieved() {

}

void ClientWorker::OnFirstRequest() { 
  QByteArray received_data, nextData;
  int separatorIndex;
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

  received_data.append(socket->readAll());

  // work on all requests if there are more than one
  //  such a separator, untill we design something better
  if (received_data.contains(Parser::GetUnpossibleSequence())) {
    separatorIndex = received_data.indexOf(Parser::GetUnpossibleSequence());

    nextData = received_data.mid(separatorIndex + 2);
    received_data = received_data.left(separatorIndex);
  }

  Message mes = Parser::ParseAsMessage(received_data);

  connections_[mes.id];
}
