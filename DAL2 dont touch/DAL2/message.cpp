#include "message.h"
namespace SQLDAL {
Messages::Messages(std::shared_ptr<Connect> Connect) : Statement(Connect) {
  connection_->Open(CLIENT_DB);
}

Messages::Messages() : Statement() {}

QVector<Message> Messages::GetMessages(unsigned int chat_id) {
  ExectuteQuery(CreateQueryCountOfMessages(chat_id));
  query_.first();
  unsigned int count_of_messages = query_.value(0).toInt();
  QVector<Message> messages(count_of_messages);
  ExectuteQuery(SelectQuery(chat_id));
  int counter = 0;
  while (query_.next()) {
    messages[counter].id = query_.record().value(0).toInt();
    messages[counter].chat_id = query_.record().value(1).toInt();
    messages[counter].owner_id = query_.record().value(2).toInt();
    messages[counter].data = query_.record().value(3).toString();
    messages[counter].date = query_.record().value(4).toString();
    messages[counter].time = query_.record().value(5).toTime();
    messages[counter++].status = query_.record().value(6).toInt();
  }
  return messages;
}

QVector<Message> Messages::GetMessages(QString user_login)
{
	ExectuteQuery(CreateQueryCountOfMessages(user_login));
	query_.first();
	unsigned int count_of_messages = query_.value(0).toInt();
	QVector<Message> messages(count_of_messages);
	ExectuteQuery(SelectQuery(user_login));
	int counter = 0;
	while (query_.next()) {
		messages[counter].id = query_.record().value(0).toInt();
		messages[counter].chat_id = query_.record().value(1).toInt();
		messages[counter].owner_id = query_.record().value(2).toInt();
		messages[counter].data = query_.record().value(3).toString();
		messages[counter].date = query_.record().value(4).toString();
		messages[counter].time = query_.record().value(5).toTime();
		messages[counter++].status = query_.record().value(6).toInt();
	}
	return messages;
}

void Messages::AddNewMessage(const Message& message) {
  ExectuteQuery(InsertQuery(message));
  query_.finish();
}

void Messages::DeleteMessage(const Message& message) {
  ExectuteQuery(DeleteQuery(message));
  query_.finish();
}

void Messages::UpdateMessage(const Message& message) {
  ExectuteQuery(UpdateQuery(message));
  query_.finish();
}

QString Messages::SelectQuery(unsigned int chat_id) {
  return QString("select * from Messages where user_ID = " +
                 QString::number(chat_id));
}

QString Messages::SelectQuery(QString user_login)
{
	return QString("select * from Messages where user_login = '" +user_login+ "'");
		
}

QString Messages::InsertQuery(const Message& messages) {
  return QString(
      "insert into Messages (user_ID, owner_ID, message_data, message_date, "
      "message_tima, message_status) values (" +
      QString::number(messages.chat_id) + ", " +
      QString::number(messages.owner_id) + ", '" + messages.data + "', '" +
      messages.date + "', '" +
      messages.time.toString() +  // FIX DATE
      "', " +
      QString::number(messages.status) + ")");
}

QString Messages::UpdateQuery(const Message& message) {
  return QString("update Messages set message_data = '" + message.data +
                 "', message_status = '" + QString::number(message.status) +
                 "'  where message_ID = " + QString::number(message.id));
}

QString Messages::DeleteQuery(const Message& message) {
  return QString("delete from Messages where message_ID = " +
                 QString::number(message.id));
}

QString Messages::CreateQueryCountOfMessages(unsigned int chat_id) {
  return QString("SELECT COUNT(user_ID) FROM Messages WHERE user_ID = " +
                 QString::number(chat_id));
}
QString Messages::CreateQueryCountOfMessages(QString user_login)
{
	return QString("SELECT COUNT(user_ID) FROM Messages WHERE user_login = '" + user_login +"'");
}
}  // namespace SQLDAL