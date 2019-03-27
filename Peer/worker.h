#ifndef WORKER_H
#define WORKER_H

#include "abstractstrategy.h"
#include "messagestrategy.h"
#include "blockreader.h"

#include <QByteArray>
#include <QObject>

#include "../Parser&Structs/parser.h"

class Worker : public QObject {
  Q_OBJECT

 public:
  Worker(BlockReader* reader);
  ~Worker();

  void DoWork();
  void SetStrategy(StrategyType strategy_type);

 signals:
  void Disconnected(unsigned id);

 private slots:
  void OnDisconnected();
  void OnReadyReadBlock();

private:
  AbstractStrategy* strategy_;
  PeerInfo peer_info_;
  QTcpSocket* socket_;
  BlockReader* reader_;
  QHash<StrategyType, AbstractStrategy*> strategies_;
};

#endif  // !WORKER_H
