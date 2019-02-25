#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegExpValidator>

#include "peer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void AppendMessage(QString);

    void OnPbSendClicked();

private:
    Ui::MainWindow *ui_;
    Peer* peer_;
};

#endif // MAINWINDOW_H
