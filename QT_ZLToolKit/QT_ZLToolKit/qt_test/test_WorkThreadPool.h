#ifndef TEST_WORKTHREADPOOL_H
#define TEST_WORKTHREADPOOL_H

/**
 * @brief WorkThreadPool，获取一个可用的线程池。
 * WorkThreadPool 是全局单例对象，和 EventPollerPool 功能几乎一致，都是管理 EventPoller 所组成的线程池。
 * EventPollerPool 为了线程安全，支持优先返回当前线程，也可以选择返回最低负荷线程； WorkThreadPool 只返回最低负荷线程。
 *
 * EventPollerPool 通常用于实时性较高的业务，比如定时器、fd网络事件等，该线程不应该被耗时业务阻塞。
 * ZLM 使用 WorkThreadPool 主要用于文件读写、DNS解析、mp4关闭等耗时的工作，完成后再通过 EventPollerPool::async 切回自己的线程。
 */
void test_WorkThreadPool() {

}

#endif // TEST_WORKTHREADPOOL_H
