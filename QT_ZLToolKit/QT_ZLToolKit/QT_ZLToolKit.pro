QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DESTDIR = bin
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

ZLT_BASE=../../
INCLUDEPATH += $$ZLT_BASE/src/

DEFINES += HAVE_MMSG_HDR

SOURCES += \
    ../../src/Network/Buffer.cpp \
    ../../src/Network/BufferSock.cpp \
    ../../src/Network/Server.cpp \
    ../../src/Network/Session.cpp \
    ../../src/Network/Socket.cpp \
    ../../src/Network/TcpClient.cpp \
    ../../src/Network/TcpServer.cpp \
    ../../src/Network/UdpServer.cpp \
    ../../src/Network/sockutil.cpp \
    ../../src/Poller/EventPoller.cpp \
    ../../src/Poller/Pipe.cpp \
    ../../src/Poller/PipeWrap.cpp \
    ../../src/Poller/SelectWrap.cpp \
    ../../src/Poller/Timer.cpp \
    ../../src/Thread/TaskExecutor.cpp \
    ../../src/Thread/WorkThreadPool.cpp \
    ../../src/Util/CMD.cpp \
    ../../src/Util/File.cpp \
    ../../src/Util/MD5.cpp \
    ../../src/Util/NoticeCenter.cpp \
    ../../src/Util/SHA1.cpp \
    ../../src/Util/SSLBox.cpp \
    ../../src/Util/SSLUtil.cpp \
    ../../src/Util/SqlPool.cpp \
    ../../src/Util/base64.cpp \
    ../../src/Util/local_time.cpp \
    ../../src/Util/logger.cpp \
    ../../src/Util/mini.cpp \
    ../../src/Util/strptime_win.cpp \
    ../../src/Util/util.cpp \
    ../../src/Util/uv_errno.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../src/Network/Buffer.h \
    ../../src/Network/BufferSock.h \
    ../../src/Network/Server.h \
    ../../src/Network/Session.h \
    ../../src/Network/Socket.h \
    ../../src/Network/TcpClient.h \
    ../../src/Network/TcpServer.h \
    ../../src/Network/UdpServer.h \
    ../../src/Network/sockutil.h \
    ../../src/Poller/EventPoller.h \
    ../../src/Poller/Pipe.h \
    ../../src/Poller/PipeWrap.h \
    ../../src/Poller/SelectWrap.h \
    ../../src/Poller/Timer.h \
    ../../src/Thread/TaskExecutor.h \
    ../../src/Thread/TaskQueue.h \
    ../../src/Thread/ThreadPool.h \
    ../../src/Thread/WorkThreadPool.h \
    ../../src/Thread/semaphore.h \
    ../../src/Thread/threadgroup.h \
    ../../src/Util/CMD.h \
    ../../src/Util/File.h \
    ../../src/Util/List.h \
    ../../src/Util/MD5.h \
    ../../src/Util/NoticeCenter.h \
    ../../src/Util/ResourcePool.h \
    ../../src/Util/RingBuffer.h \
    ../../src/Util/SHA1.h \
    ../../src/Util/SSLBox.h \
    ../../src/Util/SSLUtil.h \
    ../../src/Util/SpeedStatistic.h \
    ../../src/Util/SqlConnection.h \
    ../../src/Util/SqlPool.h \
    ../../src/Util/TimeTicker.h \
    ../../src/Util/base64.h \
    ../../src/Util/function_traits.h \
    ../../src/Util/local_time.h \
    ../../src/Util/logger.h \
    ../../src/Util/mini.h \
    ../../src/Util/onceToken.h \
    ../../src/Util/strptime_win.h \
    ../../src/Util/util.h \
    ../../src/Util/uv_errno.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
