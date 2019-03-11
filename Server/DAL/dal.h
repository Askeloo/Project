#pragma once
#include <string>
#include <map>
#include <iostream>
#include <string>
#include "../DAL/Server/serverdb.h"

#include "client.h"
//using namespace std;


class DAL
{
public:
	DAL();
	//interface associated with database access will be placed here
	void SetClient(Client);
	Client getClient(QString);
	Client getClient(quint32);
	bool Check_If_Client_exists_In_Db(Client);
	int GetClientId(Client);
	//int getSize();
	//void printDatabase();
private:
	ServerDB databse_;
	//std::map<QString, Client> database;
};