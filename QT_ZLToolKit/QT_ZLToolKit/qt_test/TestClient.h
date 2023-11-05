#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include <csignal>
#include <iostream>
#include "Util/logger.h"
#include "Network/TcpClient.h"
using namespace std;
using namespace toolkit;

/**
 * 自定义类，继承于TcpClient，并重写onConnect、onRecv等虚函数，根据需求实现相应功能
 * 程序运行5个线程：stamp thread、QT_ZLToolKit、async log、event poller 0、event poller 1
 * 调用栈：
 * 发起连接（主线程或其他线程）：test_TcpClient->TcpClient::startConnect->Socket::connect，_poller->async加入 event poller 线程异步执行连接任务。
 * 异步执行连接（event poller 线程）：EventPoller::runLoop->EventPoller::onPipeEvent->Socket::connect->Socket::connect_l->async_con_cb(SockUtil::connect)->::connect，开始连接。
 *   这里使用的是非阻塞连接，connect返回EINPROGRESS则表示正在连接，async_con_cb->_poller->addEvent给fd添加可写事件，添加成功则说明连接成功。
 *
 * 连接结果回调（eventpoller线程）：async_con_cb-》Socket::onConnected(Socket::attachEvent 添加epoll监听读事件,监听接收数据)-》con_cb-》con_cb_in-》TcpClient::onSockConnect-》TestClient::onConnect。
 *
 * 数据接收回调：TcpClient::onSockConnect-》Socket::setOnRead-》_on_read=TestClient::onRecv
 * 数据接收（eventpoller线程）：EventPoller::runLoop-》epoll_wait监听到事件-》Socket::onRead-》_on_read-》TestClient::onRecv。
 *
 * 数据发送：demo里 TcpClient::startConnect 会创建定时器：_timer，每隔2秒回调一次 TestClient::onManager ，执行数据发送，定时器超时回调是在 event poller 线程，socket跨线程安全，线程锁：Socket::_mtx_sock_fd。
 * EventPoller::runLoop->EventPoller::getMinDelay->EventPoller::flushDelayTask(_timer 定时器超时)->TestClient::onManager->SockSender::<< ->SockSender::send->SocketHelper::send->Socket::send->Socket::send_l->Socket::flushAll->Socket::flushData->BufferSendMsg::send->BufferSendMsg::send_l->sendmsg(系统调用)。
 *
 * 小结：发起tcp连接可以在任意线程，非阻塞连接任务是在 event poller 线程异步执行，连接成功会添加 epoll 事件监听数据接收，发送数据可以在任意线程，使用线程锁保证线程安全。
 */
class TestClient: public TcpClient {
public:
    using Ptr = std::shared_ptr<TestClient>;
    TestClient():TcpClient() {
        DebugL;
    }
    ~TestClient(){
        DebugL;
    }
protected:
    virtual void onConnect(const SockException &ex) override{
        //连接结果事件
        InfoL << (ex ?  ex.what() : "success");
    }
    virtual void onRecv(const Buffer::Ptr &pBuf) override{
        //接收数据事件
        DebugL << pBuf->data() << " from port:" << get_peer_port();
    }
    virtual void onFlush() override{
        //发送阻塞后，缓存清空事件
        DebugL;
    }
    virtual void onError(const SockException &ex) override{
        //断开连接事件，一般是EOF
        WarnL << ex.what();
    }
    virtual void onManager() override{
        //定时发送数据到服务器
        auto buf = BufferRaw::create();
        if(buf){
            buf->assign("[BufferRaw]\0");
            (*this) << _nTick++ << " "
                    << 3.14 << " "
                    << string("string") << " "
                    <<(Buffer::Ptr &)buf;
        }
    }
private:
    int _nTick = 0;
};

int test_TcpClient() {
    // 设置日志系统
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    TestClient::Ptr client(new TestClient());//必须使用智能指针
    client->startConnect("192.168.3.64",9090);//连接服务器

//    TcpClientWithSSL<TestClient>::Ptr clientSSL(new TcpClientWithSSL<TestClient>());//必须使用智能指针
//    clientSSL->startConnect("192.168.3.64",9090);//连接服务器

    //退出程序事件处理
    static semaphore sem;
    ///SIGINT:Ctrl+C发送信号，结束程序
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
    return 0;
}

#endif // TESTCLIENT_H
