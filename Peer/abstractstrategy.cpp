#include "abstractstrategy.h"
#include "signalredirector.h"


AbstractStrategy::AbstractStrategy() 
  : redirector_(SignalRedirector::get_instance()), 
    cache_data_(CacheData::get_instance()) {}


AbstractStrategy::AbstractStrategy(QByteArray data)
    : data_(data),
      cache_data_(CacheData::get_instance()),
      redirector_(SignalRedirector::get_instance()){
}

void AbstractStrategy::set_data(QByteArray data) { 
  data_ = data; 
}

void AbstractStrategy::set_peer_info(Friend info) { 
  peer_info_ = info; 
}

void AbstractStrategy::set_my_id(unsigned my_id) { 
  my_id_ = my_id; 
}

AbstractStrategy::~AbstractStrategy() {}

