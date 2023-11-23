//#include "mainwindow.h"
//#include <QApplication>

#include <csignal>
#include <iostream>
#include "Thread/semaphore.h"
#include "qt_test/TestClient.h"
#include "qt_test/test_stampthread.h"
#include "qt_test/test_timer.h"
#include "qt_test/test_NoticeCenter.h"
#include "qt_test/test_onceToken.h"
#include "qt_test/test_ObjectStatistic.h"
#include "qt_test/test_Any.h"

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
//    test_timer();
//    test_NoticeCenter();
//    test_onceToken();
//    test_ObjectStatistic();
    test_Any();
}
