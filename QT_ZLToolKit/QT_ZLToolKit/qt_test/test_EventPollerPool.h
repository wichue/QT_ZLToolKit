#ifndef TEST_EVENTPOLLERPOOL_H
#define TEST_EVENTPOLLERPOOL_H

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

}

#endif // TEST_EVENTPOLLERPOOL_H
