#ifndef CLIENTDB_H
#define CLIENTDB_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QObject>
#include <QPair>
#include <iostream>
#include <QDateTime>
#include <QVector>
#include <QtSql/qsqlresult.h>

namespace ClientDAL {


	struct Message
	{
		int message_id;
		int owner_id;
		QString data;
		QDate date;
		QTime time;
		bool status;
	};

	struct User {
		int user_id;
		//QString user_login;
		QString user_name;
		QString user_surname;
		//QImage user_profile_photo;
	};


	class ClientDB
	{
	public:

		ClientDB();
		// ~ClientDB();

		void                 AddNewFriend(const QString& user_login, const int user_id);

		QPair<QString, int>  GetIPPort(const QString& user_login);
		QPair<QString, int>  GetIPPort(const int& user_id);

		void                 UpdateIPPort(const QString& user_login, const QString& new_user_ip, const int& new_user_port);
		void                 UpdateIPPort(const int& user_id, const QString& new_user_ip, const int& new_user_port);

		void                 AddMessage(const Message&  message, const QString& user_login);
		void                 AddMessage(const Message&  message, const int& user_id);

		QVector<Message>	GetMessages(const QString& user_login);
		QVector<Message>	GetMessages(const int& user_id);

		void                 UpdateUserProfile(const QString& user_login, const QString& user_name, const QString& user_surname);
		void                 UpdateUserProfile(const int& user_id, const QString& user_name, const QString& user_surname);

		QVector<User>		GetFriends();
		QVector<QString>	GetFriendsLogin();

		void                 DeleteFriend(const QString& user_login);
		void                 DeleteFriend(const int& user_id);

		void                 SetFriendStatus(const QString& user_login, bool status);
		void                 SetFriendStatus(const int& user_id, bool status);

		void                 SetMessageStatusRead(const int& message_id);

		bool                 GetFriendStatus(const QString& user_login);
		bool                 GetFriendStatus(const int& user_id);



	private:
		int           CountOfFriends();
		int           CountOfMessages(const QString& user_login);
		void          CreateUserProfile(const unsigned int user_id);
		void          DeleteUserProfile(const QString& user_login);
		void          ErrorInfo(void);
		unsigned int  GetIDByLogin(const QString& user_login);
		QString		  GetLoginById(const int& user_id);


		QSqlDatabase data_base_;
		QSqlQuery    query_;
	};
}

#endif // CLIENTDB_H
