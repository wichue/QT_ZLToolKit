#ifndef TEST_THREADPOOL_H
#define TEST_THREADPOOL_H

#include <chrono>
#include "Util/logger.h"
#include "Util/onceToken.h"
#include "Util/TimeTicker.h"
#include "Thread/ThreadPool.h"

using namespace std;
using namespace toolkit;

/**
 * @brief thread_group ：线程组，移植自boost。
 * create_thread 快速创建一组线程，并指定参数和线程处理函数。
 */

/**
 * @brief ThreadPool ：线程池，可以输入functional任务至后台线程执行
 * 构造函数中创建线程组 thread_group _thread_group，从任务队列 TaskQueue<Task::Ptr> _queue 获取任务在线程池执行。
 * 线程池中所有线程共用一个任务队列 _queue,ThreadPool 无锁，但 _queue 中有锁，目前ZLM中没有 ThreadPool 应用。
 */
int test_ThreadPool() {
    //初始化日志系统
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    ThreadPool pool(thread::hardware_concurrency(), ThreadPool::PRIORITY_HIGHEST, true);

    //每个任务耗时3秒
    auto task_second = 3;
    //每个线程平均执行4次任务，总耗时应该为12秒
    auto task_count = thread::hardware_concurrency() * 4;

    semaphore sem;
    vector<int> vec;
    vec.resize(task_count);
    Ticker ticker;
    {
        //放在作用域中确保token引用次数减1
        auto token = std::make_shared<onceToken>(nullptr, [&]() {
            sem.post();
        });

        for (auto i = 0; i < task_count; ++i) {
            pool.async([token, i, task_second, &vec]() {
                setThreadName(("thread pool " + to_string(i)).data());
                std::this_thread::sleep_for(std::chrono::seconds(task_second)); //休眠三秒
                InfoL << "task " << i << " done!";
                vec[i] = i;
            });
        }
    }

    sem.wait();
    InfoL << "all task done, used milliseconds:" << ticker.elapsedTime();

    //打印执行结果
    for (auto i = 0; i < task_count; ++i) {
        InfoL << vec[i];
    }
    return 0;
}

#endif // TEST_THREADPOOL_H
