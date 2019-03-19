#include "clientlogger.h"

ClientLogger* ClientLogger::Instance() {
  static ClientLogger logger_; 
    return &logger_;
}
const char* ErrorValueNames[] = {GET_NAME(ERROR), GET_NAME(SUCCESS),
                                 GET_NAME(INFO), GET_NAME(WARNING), GET_NAME(DEBUG)};

ClientLogger::ClientLogger() {
  if (!file_) {
    file_ = new QFile("Log.txt");
    file_->open(QIODevice::Append | QIODevice::Text);
  }
}
void ClientLogger::WriteLog(LogType type, const QString& msg) { 
  QString text = msg;
    text = tr("[%1] %2 |")
                .arg(ErrorValueNames[type])
          .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ")) + text + '\n';

  QTextStream out(file_);
  if (file_) {
    out << text;
  }
  emit DisplayLog(ErrorValueNames[type], msg);
}

