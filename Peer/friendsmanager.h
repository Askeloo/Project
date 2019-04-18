#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "../Parser&Structs/parser.h"
#include "abstractstrategy.h"
#include "messagestrategy.h"
 
#include "worker.h"
#include "application_info.h"
#include "clientlogger.h"

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QSet>
#include <QTcpSocket>

class FriendsManager : public QObject {
  Q_OBJECT

 public:
  FriendsManager(ApplicationInfo& info);
  ~FriendsManager();
  void AddMessageToSend(unsigned id, QString message);
  void CleanUp();
 private:
  void ReallySendMessages(unsigned id);
signals:
  void MessagesSent(unsigned);

 public slots:
  void SendMessages(const unsigned& id);
 private slots:
  void OnFirstRequestRecieved();

  void OnDisconnected(unsigned id);
  void OnConnected(unsigned id);
  void OnError(unsigned id);

 private:
  ApplicationInfo& app_info;
  QHash<unsigned, Worker*> connecting_workers_;
  QHash<unsigned, Worker*> workers_;
  QMap<unsigned, QList<QString>> pending_messages_; 
  ClientLogger* logger_;

  DataAccessor client_data_;
};

#endif  // !CLIENTMANAGER_H
