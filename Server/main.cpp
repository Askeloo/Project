#include <QtCore/QCoreApplication>
#include "AsyncServer.h"
#include <iostream>
//#include <windows.h>
 // ALL COMMeNTS Below IS FOR HIDING CONSOLe WINDOW THAT FOR FUTURe SO DONT Touch
int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
	AsyncServer async_server; // run server
  //HWND hWnd = GetConsoleWindow();
  //ShowWindow(hWnd, SW_HIDE);

 return a.exec();
}
