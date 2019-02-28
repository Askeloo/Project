#include "clientdb.h"


ClientDB::ClientDB()
{
    data_base_=QSqlDatabase::addDatabase("QSQLITE");
    data_base_.setDatabaseName("../DAL/Client/Client");

    if (!data_base_.open())
    {
        qDebug()<<data_base_.lastError().text();
    }
    else
    {
        qDebug() << "success";
    }

    query_ = QSqlQuery(data_base_);

    if (!query_.exec("select * from friends"))
    {
        ErrorInfo();
    }
    else
    {
        qDebug()<<"success";
    }
}

void ClientDB::AddNewFriend(const QString &user_login, const int user_id)
{
    query_.prepare("INSERT INTO friends (user_id, user_login) VALUES (:new_user_id, :new_user_login)");
        query_.bindValue(":new_user_id",user_id);
        query_.bindValue(":new_user_login",user_login);
        if (!query_.exec())//TODO: exception if bad
        {
            ErrorInfo();
        }
    CreateUserProfile(user_id);
}

QPair<QString, int> ClientDB::GetIPPort(const QString &user_login)
{
    query_.prepare("select user_IP,user_port from friends where user_login = :user_login");
    query_.bindValue(":user_login", user_login);
    QPair<QString, int> result_query;
    if (query_.exec())
    {
        while (query_.next())
        {
             result_query.first = query_.record().value(0).toString();
             result_query.second = query_.record().value(1).toInt();
        }
    }
    else
    {
       ErrorInfo();
    }
    return result_query;
}

void ClientDB::UpdateIPPort(const QString &user_login, const QString &new_user_ip, const int &new_user_port)
{
    query_.prepare("UPDATE friends SET user_IP = :new_user_ip, user_port = :new_user_port WHERE user_login  = :user_login ");
    query_.bindValue(":user_login", user_login);
    query_.bindValue(":new_user_ip",new_user_ip);
    query_.bindValue(":new_user_port",new_user_port);
    if (!query_.exec())//TODO: exception if bad
    {
       ErrorInfo();
    }
}

void ClientDB::AddMessage(const Message &message, const QString& user_login)
{

        int user_id = GetIDByLogin(user_login);
        query_.prepare("INSERT INTO Messages (user_ID, owner_ID, message_data,message_date,message_tima, message_status) VALUES ( :new_user_ID , :new_owner_ID, :new_message_data, :new_message_date,:new_message_time,:new_message_status)");
        query_.bindValue(":new_user_ID", user_id);
        query_.bindValue(":new_owner_ID", message.owner_id);
        query_.bindValue(":new_message_data", message.data);
        query_.bindValue(":new_message_date", message.date.toString());
        query_.bindValue(":new_message_time", message.time.toString());
        query_.bindValue(":new_message_status", message.status);

        if (!query_.exec())//TODO: exception if bad
        {
            ErrorInfo();
        }
}

std::vector<Message> ClientDB::GetMessages(const QString &user_login)
{
    int count_of_messages = CountOfMessages(user_login);

    unsigned int user_id = GetIDByLogin(user_login);
    std::vector<Message> messages(count_of_messages);
    query_.prepare("SELECT owner_ID,message_data,message_date,message_tima, message_ID FROM Messages WHERE user_ID = :user_id ");
    query_.bindValue(":user_id", user_id );
    int counter = 0;
    if (query_.exec())
    {
        while (query_.next())
        {
             messages[counter].owner_id = query_.record().value(0).toInt();
             messages[counter].data = query_.record().value(1).toString();
             messages[counter].date = query_.record().value(2).toDate();
             messages[counter].time = query_.record().value(3).toTime();
             messages[counter++].message_id= query_.record().value(4).toInt();
        }
    }
    else
    {
       ErrorInfo();
    }

    return messages;
}

void ClientDB::UpdateUserProfile(const QString &user_login, const QString &user_name, const QString &user_surname)
{
    int user_id = GetIDByLogin(user_login);
    query_.prepare("UPDATE friend_info SET user_name = :new_user_name, user_surname = :new_user_surname WHERE user_ID  = :user_id ");
    query_.bindValue(":user_id", user_id);
    query_.bindValue(":new_user_name",user_name);
    query_.bindValue(":new_user_surname",user_surname);
    if (!query_.exec())//TODO: exception if bad
    {
       ErrorInfo();
    }
}

std::vector<User> ClientDB::GetFriends()
{
    unsigned int count_of_friends = CountOfFriends();


    std::vector<User>friends(count_of_friends);
    query_.prepare("SELECT user_ID, user_name, user_surname FROM friend_info");

    int counter = 0;
    if (query_.exec())
    {
        while (query_.next())
        {
             friends[counter].user_id = query_.record().value(0).toInt();
             friends[counter].user_name = query_.record().value(1).toString();
             friends[counter++].user_surname = query_.record().value(2).toString();
        }
    }
    else
    {
       ErrorInfo();
    }

    return friends;
}

void ClientDB::DeleteFriend(const QString &user_login)
{
    DeleteUserProfile(user_login);

    query_.prepare("DELETE FROM friends WHERE user_login  = :user_login ");
    query_.bindValue(":user_login", user_login);

    if (!query_.exec())//TODO: exception if bad
    {
       ErrorInfo();
    }

}

void ClientDB::SetFriendStatus(const QString &user_login, bool status)
{
    int user_id = GetIDByLogin(user_login);
    query_.prepare("UPDATE friends SET user_status = :new_user_status WHERE user_ID  = :user_id ");
    query_.bindValue(":user_id", user_id);
    query_.bindValue(":new_user_status", status);
    if (!query_.exec())
    {
       ErrorInfo();
    }
}

void ClientDB::SetMessageStatusRead(const int &message_id)
{
    query_.prepare("UPDATE Messages SET message_status = true WHERE message_ID  = :message_id");
    query_.bindValue(":message_id", message_id);
    if (!query_.exec())
    {
       ErrorInfo();
    }
}

bool ClientDB::GetFriendStatus(const QString &user_login)
{
    query_.prepare("SELECT user_status FROM friends WHERE user_login  = :user_login ");
        query_.bindValue(":user_login",user_login);
        bool status = false;
        if (query_.exec())
        {
            while (query_.next())
            {
                 status = query_.record().value(0).toBool();
            }
        }
        else
        {
           ErrorInfo();
        }
        return status;
}

int ClientDB::CountOfFriends()
{
    int count_of_friends = 0;
    query_.prepare("SELECT COUNT(user_ID) FROM friend_info");
    if (query_.exec())
    {
        while (query_.next())
        {
             count_of_friends = query_.record().value(0).toInt();
        }
    }
    else
    {
       ErrorInfo();
    }
    return count_of_friends;
}

int ClientDB::CountOfMessages(const QString &user_login)
{
    unsigned int count_of_messages = 0;
    unsigned int user_id = GetIDByLogin(user_login);
    query_.prepare("SELECT COUNT(user_ID) FROM Messages WHERE user_ID = :user_id ");
    query_.bindValue(":user_id", user_id );
    if (query_.exec())
    {
        while (query_.next())
        {
             count_of_messages = query_.record().value(0).toUInt();
        }
    }
    else
    {
       ErrorInfo();
    }
    return count_of_messages;
}






void ClientDB::CreateUserProfile(const unsigned int user_id)
{
    query_.prepare("INSERT INTO friend_info (user_ID) VALUES ( :new_user_ID)");
    query_.bindValue(":new_user_ID", user_id);
    //query_.bindValue(":new_user_name", user_name);
    //query_.bindValue(":new_user_surname", user_surname);

    if (!query_.exec())//TODO: exception if bad
    {
        ErrorInfo();
    }
}

void ClientDB::DeleteUserProfile(const QString &user_login)
{
    uint user_id = GetIDByLogin(user_login);

    query_.prepare("DELETE FROM friend_info WHERE user_ID  = :user_id ");
    query_.bindValue(":user_id", user_id);

    if (!query_.exec())//TODO: exception if bad
    {
       ErrorInfo();
    }


}


void ClientDB::ErrorInfo()
{
    qDebug() << query_.lastError().databaseText();
    qDebug() << query_.lastError().driverText();
}

unsigned int ClientDB::GetIDByLogin(const QString &user_login)
{
         query_.prepare("select user_id from friends where user_login = :user_login");
         query_.bindValue(":user_login", user_login);

         unsigned int id = 0;
         if (query_.exec())
         {
             while (query_.next())
             {
                  id = query_.record().value(0).toInt();
             }
         }
         else
         {
            ErrorInfo();
         }
         return id;
}
