#include "guimanager.h"

#include <chrono>
#include <thread>
#include <QtConcurrent>



GUIManager::GUIManager(QObject *parent)
    : QObject(parent) {
  LoadFriends();

}

FriendModel* GUIManager::friendModel() {
  return  &friend_model_;
}

void GUIManager::newFriend() {
  FriendItem* new_friend = new FriendItem("oleksyk", true);
  friend_model_.addFriendToList(new_friend);
}

void GUIManager::deleteFriend(FriendItem* friend_to_delete) {
  if (!friend_to_delete)
      return;

  friend_model_.removeFriendFromList(friend_to_delete);
}





void GUIManager::logIn() {
  std::this_thread::sleep_for(std::chrono::seconds(10));
  qDebug() << "Hello from c++";
}

void GUIManager::logIn2() {
  QtConcurrent::run(this, &GUIManager::logIn);
}

void GUIManager::LoadFriends() {
  QVector<SQLDAL::Friend> friends = client_data_.GetFriends();

  for (const SQLDAL::Friend& i : friends) {
    bool status = client_data_.GetFriendStatus(client_data_.GetIDByLogin(i.login));  // just for test
    FriendItem* friend_item = new FriendItem(i.login, status);
    friend_model_.addFriendToList(friend_item);
  }
}
