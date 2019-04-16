#include "guimanager.h"

GUIManager::GUIManager(QObject *parent)
    : QObject(parent),
      logger_(ClientLogger::Instance()) {     //for testing
  controller_ = new ClientController(this);
  logger_->set_log_level(LogLevel::HIGH);

  connect(this, SIGNAL(SelectedFriendIdChanged(unsigned)), this, SLOT(ShowMessages(unsigned)));
  connect(controller_, SIGNAL(MessageRecieved(Message*)), this, SLOT(LoadMessage(Message*)));
  connect(controller_, SIGNAL(LoginResult(bool)), this, SLOT(OnLoginResult(bool)));
  connect(controller_, SIGNAL(RegisterResult(quint32)), this, SLOT(OnRegisterResult(quint32)));
  connect(controller_, SIGNAL(StatusChanged(unsigned, bool)), this,
          SLOT(OnStatusChanged(unsigned, bool)));
  connect(controller_, SIGNAL(FriendRequestResult(bool)), this,
          SLOT(OnFriendRequestResult(bool)));
  connect(controller_, SIGNAL(AddFriendRequestInfo(QString)), this, 
          SLOT(OnAddFriendRequest(QString)));
  connect(controller_, SIGNAL(NewFriendRequestResult(QString, quint32)), this,
          SLOT(OnNewFriendInfo(QString, quint32)));
}

int GUIManager::my_id() const { 
  return controller_->app_info_.my_id;
}

FriendModel* GUIManager::friend_model() {
  return  &friend_model_;
}

MessageModel* GUIManager::message_model() {
  return &message_model_;
}

FriendRequestModel *GUIManager::friend_request_model() {
  return &friend_request_model_;
}

unsigned GUIManager::selected_friend_id() { 
  return selected_friend_id_; 
}

void GUIManager::set_selected_friend_id(unsigned selected_id) {
  if (selected_friend_id_ == selected_id) return;

  selected_friend_id_ = selected_id;
  emit SelectedFriendIdChanged(selected_id);
}

void GUIManager::newFriend(QString new_friend_login, quint32 id) {
  FriendItem* new_friend = new FriendItem(new_friend_login, false, id); //FIXME: id
  friend_model_.AddFriendToList(new_friend);
}

void GUIManager::deleteFriend(FriendItem* friend_to_delete) {
  if (!friend_to_delete)
      return;
  friend_model_.RemoveFriendFromList(friend_to_delete);
}

void GUIManager::newMessage(QString message) {
  Message* temp = new Message{0, selected_friend_id_, controller_->app_info_.my_id, message,
                              QDate::currentDate(), QTime::currentTime()};
  messages_cache_[selected_friend_id_].push_back(temp);
  MessageItem* new_message = new MessageItem(temp);
  message_model_.AddMessageToList(new_message);
}

void GUIManager::ShowMessages(unsigned friend_id) {
  if (friend_id) {
    message_model_.RemoveAllMessagesFromList();

    MessageItem* new_message;

    if (messages_cache_.find(friend_id) == messages_cache_.end()) {
      LoadAllMessages(friend_id);
    }

    for (Message* msg : messages_cache_[friend_id]) {
      new_message = new MessageItem(msg);
      message_model_.AddMessageToList(new_message);
    }
    friend_model_.DeleteUnreadMesgs(friend_id);
  }
}

void GUIManager::LoadAllMessages(unsigned friend_id) {
  Message* temp;
  QList<Message*> messages;
  QVector<Message> history = controller_->LoadMessages(friend_id);  //use controller's func  
  for (const auto& msg : history) {
    temp = new Message(msg);
    messages.push_back(temp);
  }
  messages_cache_[friend_id] = messages;
}

void GUIManager::LoadMessage(Message* msg) {
  MessageItem* new_message;

  messages_cache_[msg->chat_id].push_back(msg);
  if (selected_friend_id_ == msg->chat_id) {
    new_message = new MessageItem(msg);
    message_model_.AddMessageToList(new_message);
  } else {
    friend_model_.AddUnreadMesg(msg->chat_id);
  }
}

void GUIManager::deleteFriendRiequest(FriendRequestItem* friend_request_to_delete) {
  if (!friend_request_to_delete)
      return;
  friend_request_model_.RemoveRequestFromList(friend_request_to_delete);
}

void GUIManager::newFriendRiequest(QString login) {
  FriendRequestItem* new_friend_request = new FriendRequestItem(login);
  friend_request_model_.AddRequestToList(new_friend_request);
}

void GUIManager::LogIn(QString user_login, QString user_password) { 
  controller_->app_info_.remote_server_ip = "192.168.195.144";
  controller_->app_info_.remote_server_port = 8888;
  controller_->app_info_.my_port = 8989;  //FIXME
  controller_->app_info_.my_login = user_login;
  controller_->app_info_.my_password = user_password;
  controller_->app_info_.my_id = client_data_.get_id_by_login(user_login);  //FIXME
  logger_->WriteLog(LogType::SUCCESS, user_login);
  controller_->LogIn(user_login, user_password);
  //OnLoginResult(true);
}

void GUIManager::Register(QString user_login, QString user_password) {
  controller_->app_info_.remote_server_ip = "192.168.195.144";
  controller_->app_info_.remote_server_port = 8888;
  controller_->app_info_.my_port = 8989;  // FIXME
  controller_->Register(user_login, user_password);
}

void GUIManager::OnLoginResult(bool logged_in) {
  if (logged_in) {
    LoadFriends();

    ShowMessages(friend_model_.GetFirstFriendId());
    selected_friend_id_ = friend_model_.GetFirstFriendId();

    logger_->WriteLog(LogType::SUCCESS, controller_->app_info_.my_login);
    emit openMainPage();
  }
  else {
    emit openFailed("Log In");
  }
}

void GUIManager::OnRegisterResult(quint32 new_id) {
  if (new_id) {
    selected_friend_id_ = friend_model_.GetFirstFriendId();
    emit openMainPage();
  }
  else {
    emit openFailed("Register");
  }
}

void GUIManager::OnStatusChanged(unsigned id, bool status) {
  friend_model_.SetStatus(id, status);
}

void GUIManager::AddFriendRequest(QString new_friend_login) {
  controller_->AddFriend(new_friend_login);
}

void GUIManager::OnFriendRequestResult(bool request_result) {
  if(request_result){
    emit showInfo("Friend request sent");
    logger_->WriteLog(LogType::SUCCESS, "Good requst");
  } else {
    emit showInfo("Bad friend request");
    logger_->WriteLog(LogType::ERROR, "Bad requst");
  }
}

void GUIManager::OnAddFriendRequest(QString login) {
  emit showInfo(login + " want to add you to friends");
  newFriendRiequest(login);
}

void GUIManager::AcceptFriend(QString login) {
  controller_->FriendRequestAccepted(login);
}

void GUIManager::RejectFriend(QString login) {
  controller_->FriendRequestRejected(login);  
}

void GUIManager::OnNewFriendInfo(QString login, quint32 id) {
  logger_->WriteLog(LogType::SUCCESS, "User with login '" + login + "' added");
  emit showInfo("User " + login + " added to your friend list");
  newFriend(login, id);
}

void GUIManager::SendMessage(QString message) { 
  if (selected_friend_id_) {
    controller_->SendMessage(selected_friend_id_, message);
    newMessage(message);
  }
}

void GUIManager::LoadFriends() {   //don't forget to load id
  for (const Friend& i : controller_->LoadFriends()) {
    if (i.id == controller_->app_info_.my_id) continue;
    FriendItem* friend_item = new FriendItem(i.login, false, i.id);
    friend_model_.AddFriendToList(friend_item);
  }
}

