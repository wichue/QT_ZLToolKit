#ifndef TEST_ONCETOKEN_H
#define TEST_ONCETOKEN_H

#include <csignal>
#include "Util/onceToken.h"
#include "Poller/EventPoller.h"
using namespace std;
using namespace toolkit;

/**
 * RAII [1] （Resource Acquisition Is Initialization）,也称为“资源获取就是初始化”，是C++语言的一种管理资源、避免泄漏的惯用法。
 * RAII的思想：构造时获取资源，在对象生命周期内保持资源有效，最后对象析构时释放资源。
 *
 * @brief onceToken:使用RAII模式实现，可以在对象构造和析构时执行一段代码。
 * 也就是在构造时执行一段代码（传nullptr则什么都不执行），在离开作用域时执行一段代码。
 * 在ZLM中，onceToken 主要用于防止在程序抛出异常时提前返回，没有执行接下来的代码。
 * 把一定要执行的代码放在 onceToken 析构时执行，防止程序抛出异常提前返回。
 * 如果要等待异步执行后再析构，在执行 async 时把 onceToken 智能指针作为行参传递给Lambda表达式。
 */

void token_start() {
    printf("token start\n");
}

void test_onceToken() {
    EventPoller::Ptr poller = EventPollerPool::Instance().getPoller();

    //异步执行时传递 onceToken 智能指针行参，引用计数加1，等所有的异步执行结束后引用计数变为0，执行析构
    /**
      * 打印：
      * token start
      * async=0
      * async=1
      * async=2
      * token destruct
      */
    {
        auto token = std::make_shared<onceToken>(token_start, []() {
            printf("token destruct\n");
        });

        for (auto i = 0; i < 3; ++i) {
            poller->async([token, i]() {//EventPoller 线程异步执行
                printf("async=%d\n",i);
                std::this_thread::sleep_for(std::chrono::seconds(1)); //休眠1秒
            });
        }
    }

    //退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
}


#endif // TEST_ONCETOKEN_H
