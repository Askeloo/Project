#ifndef FRIENDITEM_H
#define FRIENDITEM_H

#include <QObject>

class FriendItem : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString login READ login WRITE set_login NOTIFY LoginChanged)
  Q_PROPERTY(bool status READ status WRITE set_status NOTIFY StatusChanged)

public:
  FriendItem(QString login, bool status, QObject *parent = nullptr);

  QString login() const;
  void set_login(QString);
  bool status() const;
  void set_status(bool);

signals:
  void LoginChanged();
  void StatusChanged();

private:
  QString login_;
  bool status_;
};
#endif // FRIENDITEM_H
