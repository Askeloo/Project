#ifndef MESSAGE_H
#define MESSAGE_H
#include <qsqlrecord.h>
#include <qvector.h>
#include <QDate>
#include <QString>
#include <QTime>
#include "statement.h"
namespace SQLDAL {
struct Messages {
  unsigned int id;
  unsigned int chat_id;
  unsigned int owner_id;
  QString data;
  QDate date;
  QTime time;
  bool status;
};
class Message : public Statement {
 public:
  Message(std::shared_ptr<Connect> Connect);
  Message();
  QVector<Messages> messages;
  void GetMessages(unsigned int chat_id);  // Select All Info About One Chat
  void AddNewMessage(Messages message);
  void DeleteMessage(Messages message);
  void UpdateMessage(Messages message);

 private:
  QString UpdateQuery(Messages message);
  QString SelectQuery(unsigned int chat_id);
  QString InsertQuery(Messages message);
  QString DeleteQuery(Messages message);
  QString CreateQueryCountOfMessages(unsigned int chat_id);
};
}  // namespace SQLDAL
#endif  // MESSAGE_H
