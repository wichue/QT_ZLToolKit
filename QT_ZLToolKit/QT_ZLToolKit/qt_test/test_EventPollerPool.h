#ifndef TEST_EVENTPOLLERPOOL_H
#define TEST_EVENTPOLLERPOOL_H

#include <csignal>
#include <iostream>
#include "Util/logger.h"
#include "Network/TcpClient.h"
using namespace std;
using namespace toolkit;

/**
 * @brief EventPollerPool，获取一个可用的线程池。
 * EventPollerPool 是全局单例对象，用来管理多个 EventPoller 组成的线程池，后者是基于 epoll 实现的线程。
 * EventPoller 线程主要处理：定时器(Timer)、异步任务(async)、网络事件(socket)，epoll 监听管道fd和 socket fd 事件，加入 _event_map<fd,CB>。
 * 定时器(Timer)：可由任意线程调用，线程安全，异步加入延时任务队列 _delay_task_map<触发时间(ms),Task> ，距离最近触发定时器时间传入 epoll_wait 的超时时间，每次循环检测定时器队列，触发回调。
 * 异步任务(async):可由任意线程调用，任务加入 _list_task<Task> 队列，有锁线程安全，并通过写管道 _pipe 唤醒epoll线程执行任务。
 * 网络事件(socket)：EventPoller 智能指针可以和 socket 绑定，监听处理fd接收/断开/错误等网络事件；socket 数据发送可以在单线程或任意线程进行(enable_mutex)，线程锁 _mtx_sock_fd 。
 *
 * EventPoller 对象 只能在 EventPollerPool 中构造，EventPollerPool 管理 EventPoller 线程池。
 * EventPollerPool 负责创建和管理 EventPoller 对象， 可获取当前EventPoller线程，最低负荷EventPoller线程，第一个EventPoller线程等，也可以自定义规则获取EventPoller线程对象。
 */
void test_EventPollerPool() {

    //全局单例，获取实例即执行构造,addPoller 创建线程池，线程保存在其基类成员： std::vector<TaskExecutor::Ptr> _threads;
    //默认创建线程个数=CPU个数，也可以 setPoolSize 设置线程个数
    EventPollerPool::Instance();

    //从线程池返回一个 EventPoller 线程的智能指针,增加其引用计数
    //可以选择优先返回当前线程，或返回最低负荷线程
    std::shared_ptr<EventPoller> poller1 = EventPollerPool::Instance().getPoller();
    std::shared_ptr<EventPoller> poller2 = EventPollerPool::Instance().getPoller();
    printf("use_count=%ld\n",poller1.use_count());//use_count=3

    printf("main threadid=%ld\n",pthread_self());
    //异步执行，可以在任意线程调用,lambda 表达式在 EventPoller 线程异步执行
    //通过 lambda 表达式传参，把 lambda 这个匿名函数加入 EventPoller 线程的 List<Task::Ptr> _list_task 任务队列,Task 无参无返回值。
    int num = 15;
    poller1->async([num](){
        printf("poller1 threadid=%ld,num=%d\n",pthread_self(),num);
    });

    //定时器(Timer)参考： test_timer.h
    //网络事件(socket)参考： TestClient.h

    /**
     * 打印：
     * use_count=3
     * main threadid=139907182602176
     * poller1 threadid=139907174205184,num=15
     */

    //退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
}

#endif // TEST_EVENTPOLLERPOOL_H
