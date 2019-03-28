#ifndef ABSTRACTSTRATEGY_H
#define ABSTRACTSTRATEGY_H

#include "cachedata.h"
#include "blockwriter.h"
#include "dataaccessor.h"
#include "peerinfo.h"

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

#include "../Parser&Structs/parser.h"

class SignalRedirector;

enum StrategyType : quint8 {
  SEND_MESSAGE,
  RECEIVE_MESSAGE,
  LOGIN,
  REGISTER,
  FIND_FRIEND,
  ADD_FRIEND
};

class AbstractStrategy : public QObject {
  Q_OBJECT

 public:
  AbstractStrategy();
  AbstractStrategy(QByteArray data);
  void set_data(QByteArray data);
  void set_peer_info(PeerInfo info);
  virtual void DoWork() = 0;
  virtual ~AbstractStrategy();

 protected:
  PeerInfo peer_info_;
  QByteArray data_;
  DataAccessor client_data_;
  SignalRedirector& redirector_;
  CacheData& cache_data_;
};

#endif  // !ABSTRACTSTRATEGY_H
