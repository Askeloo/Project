#include "message.h"
namespace SQLDAL {
Message::Message(std::shared_ptr<Connect> Connect) : Statement(Connect) {
  connection_->Open(CLIENT_DB);
}

Message::Message() : Statement() {}

void Message::GetMessages(unsigned int chat_id) {
  ExectuteQuery(CreateQueryCountOfMessages(chat_id));
  query_.first();
  unsigned int count_of_messages = query_.value(0).toInt();
  messages.resize(count_of_messages);
  ExectuteQuery(SelectQuery(chat_id));
  int counter = 0;
  while (query_.next()) {
    messages[counter].id = query_.record().value(0).toInt();
    messages[counter].chat_id = query_.record().value(1).toInt();
    messages[counter].owner_id = query_.record().value(2).toInt();
    messages[counter].data = query_.record().value(3).toString();
    messages[counter].date = query_.record().value(4).toDate();
    messages[counter].time = query_.record().value(5).toTime();
    messages[counter++].status = query_.record().value(6).toInt();
  }
}

void Message::AddNewMessage(Messages mes) {
  ExectuteQuery(InsertQuery(mes));
  query_.finish();
}

void Message::DeleteMessage(Messages mes) {
  ExectuteQuery(DeleteQuery(mes));
  query_.finish();
}

void Message::UpdateMessage(Messages mes) {
  ExectuteQuery(UpdateQuery(mes));
  query_.finish();
}

QString Message::SelectQuery(unsigned int chat_id) {
  return QString("select * from Messages where user_ID = " +
                 QString::number(chat_id));
}

QString Message::InsertQuery(Messages messages) {
  return QString(
      "insert into Messages (user_ID, owner_ID, message_data, message_date, "
      "message_tima, message_status) values (" +
      QString::number(messages.chat_id) + ", " +
      QString::number(messages.owner_id) + ", '" + messages.data + "', '" +
      messages.date.toString() + "', '" + messages.time.toString() + "', " +
      QString::number(messages.status) + ")");
}

QString Message::UpdateQuery(Messages mes) {
  return QString("update Messages set message_data = '" + mes.data +
                 "', message_status = '" + QString::number(mes.status) +
                 "'  where message_ID = " + QString::number(mes.id));
}

QString Message::DeleteQuery(Messages mes) {
  return QString("delete from Messages where message_ID = " +
                 QString::number(mes.id));
}

QString Message::CreateQueryCountOfMessages(unsigned int chat_id) {
  return QString("SELECT COUNT(user_ID) FROM Messages WHERE user_ID = " +
                 QString::number(chat_id));
}
}  // namespace SQLDAL