#include "friend_update_strategy.h"
#include "signalredirector.h"

FriendUpdateStrategy::FriendUpdateStrategy()
: AbstractStrategy()
{
}

FriendUpdateStrategy::~FriendUpdateStrategy()
{}

void FriendUpdateStrategy::DoWork() { 
  info_ = Parser::ParseAsFriendUpdateInfo(data_);
  dal.UpdateIPPort(info_.id, info_.ip.toString(), info_.port);
}
