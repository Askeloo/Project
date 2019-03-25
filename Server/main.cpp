#include <QtCore/QCoreApplication>
#include "AsyncServer.h"
#include <iostream>
#include"sqldal.h"


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	UnitOfWork obj{};
	auto user = obj.GetEntity<User>();

	user->id = 4;
	user->GetUser();
	std::cout << user->login.toStdString();
	//AsyncServer async_server; // run server
	return a.exec();
}
