#pragma once
#include <iostream>
#include <map>
#include <string>
#include "../DAL/Server/serverdb.h"
#include"../Server/Exceptions/user_not_found.h"

#include <QUuid>
#include "client.h"
         
// using namespace std;

class DAL {
 public:
  DAL();
  ~DAL();
  // interface associated with database access will be placed here
  void CreateNew(Client);
  void SetClientIpPort(Client);
  Client getClient(QString);
  Client getClient(quint32);
  bool Check_If_Client_exists_In_Db(Client);
  bool Check_If_Client_exists_In_Db(QString);

  int GetClientId(Client);

 private:
  ServerDB database_;
  QString connection_name_;
  // std::map<QString, Client> database;
};
