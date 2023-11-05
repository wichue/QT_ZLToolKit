#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#include <csignal>
#include <iostream>
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/TimeTicker.h"
#include "Poller/Timer.h"

using namespace std;
using namespace toolkit;

/**
 * 测试定时器
 * Ticker类：可以统计代码执行时间，一般的计时统计
 * Timer类：定时器类，构造函数传参超时时长、回调函数等，根据回调函数判断是否重复下次任务，回调函数在event poller线程异步执行
 * 程序运行5个线程：stamp thread、QT_ZLToolKit、async log、event poller 0、event poller 1
 *
 * 调用栈
 * 添加定时器(任意线程)：Timer::Timer-》EventPoller::doDelayTask-》async_first(异步执行，把定时器任务添加到 _delay_task_map)-》EventPoller::async_l(_list_task.emplace_front)
 * 定时器超时：EventPoller::runLoop-》EventPoller::getMinDelay-》EventPoller::flushDelayTask-》(*(it->second))()，在这里执行 Timer 构造函数第二个参数传的回调函数。
 * 实现原理：创建定时器时把延迟触发时间和回调函数传参加入 _delay_task_map，时间会加上当前时间(now+delay_ms)，event poller 线程轮询所有定时器，比较当前时间now与上述 _delay_task_map 里的时间，超时则执行回调函数。
 * 实现技巧：先看 event poller 线程的唤醒，如果是网络事件(比如接收tcp数据)可以直接唤醒 epoll_wait ，新加入异步任务是通过管道唤醒；
 * Timer 定时器任务则是依赖 event poller 线程轮询检测是否超时，那多久检测一次(也就是 epoll_wait 超时时间)？这里在每次执行 getMinDelay 时会把最近定时器超时时长作为返回值，传参给 epoll_wait，下次线程唤醒时正好最近的定时器超时，执行任务；
 * 这样既保证线程不会过度轮询浪费cpu资源，也可以保证定时器任务能尽快执行。
 * EventPoller::doDelayTask 加入 _delay_task_map 是在 event poller 线程异步执行，通过写管道唤醒 event poller 线程，可以刷新 minDelay=getMinDelay 时间，也就是下次唤醒 epoll_wait 的时间。
 */
void test_timer() {
    //设置日志
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());


    Ticker ticker0;
    Timer::Ptr timer0 = std::make_shared<Timer>(0.5f,[&](){
        TraceL << "timer0重复:" << ticker0.elapsedTime();
        ticker0.resetTime();
        return true;
    }, nullptr);

    Timer::Ptr timer1 = std::make_shared<Timer>(1.0f,[](){
        DebugL << "timer1不再重复";
        return false;
    },nullptr);

    Ticker ticker2;
    Timer::Ptr timer2 = std::make_shared<Timer>(2.0f,[&]() -> bool {
        InfoL << "timer2,测试任务中抛异常" << ticker2.elapsedTime();
        ticker2.resetTime();
        throw std::runtime_error("timer2,测试任务中抛异常");
    },nullptr);

    //退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
}

#endif // TEST_TIMER_H
