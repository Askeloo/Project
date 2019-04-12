#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "../Parser&Structs/request_types.h"
#include "abstractstrategy.h"
#include "blockreader.h"
#include "blockwriter.h"
#include "clientlogger.h"
#include "friend_update_strategy.h"
#include "login_response_strategy.h"
#include "messagestrategy.h"
#include "register_response_strategy.h"
#include "friend_request_response_strategy.h"
#include "add_friend_request_strategy.h"
#include "new_friend_response_strategy.h"
#include "application_info.h"
#include <QObject>
#include <QMap>
#include <memory>
#
class ServerManager : public QObject {
  Q_OBJECT

 public:
  ServerManager(QTcpSocket* socket, ApplicationInfo& info);
  ServerManager();
  ~ServerManager();
  void set_socket(QTcpSocket* socket);
  void SendRequest(QByteArray data);

 private slots:
  //void OnNewConnection();
  void DoWork();
  void OnReadyReadBlock();
  void OnConnected();

 private:
  QByteArray data_;
  ApplicationInfo& app_info_;
  AbstractStrategy* strategy_;
  QMap<ServerRequest, AbstractStrategy*> strategies_;
  BlockWriter* writer_;
  BlockReader* reader_;
  QTcpSocket* socket_;
  ClientLogger* logger_;
};

#endif  // !SERVERMANAGER_H
