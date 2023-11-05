//#include "mainwindow.h"
//#include <QApplication>

#include <csignal>
#include <iostream>
#include "Thread/semaphore.h"
#include "qt_test/TestClient.h"
#include "qt_test/test_stampthread.h"
#include "qt_test/test_timer.h"

int main(int argc, char *argv[])
{
#if 0
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
#endif

    setbuf(stdout,nullptr);
//    test_TcpClient();
//    test_stampthread();
    test_timer();
}
