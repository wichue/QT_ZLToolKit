## Thread
### semaphore.h(自定义信号量，封装类，由条件变量实现)
class semaphore，接口：post、wait。

### TaskExecutor.h(cpu负载计算，Task函数指针模板，任务执行器管理，管理任务执行线程池)
class ThreadLoadCounter，cpu负载计算器，基类，统计线程每一次的睡眠时长和工作时长，并记录样本，调用load计算cpu负载=工作时长/总时长。
class TaskCancelable : public noncopyable，抽象类，可取消任务基类。
class TaskCancelableImp<R(ArgTypes...)> : public TaskCancelable，函数指针模板，event poller async 任务、DelayTask 任务等均使用该类型，任务可取消，重载()运算符执行任务，根据返回值类型，返回默认返回值。
class TaskExecutorInterface，抽象类，提供任务执行接口：async、async_first、sync、sync_first。
class TaskExecutor : public ThreadLoadCounter, public TaskExecutorInterface，任务执行器，抽象类，无新增接口。
class TaskExecutorGetter，获得任务执行器，抽象类，接口：getExecutor、getExecutorSize。
class TaskExecutorGetterImp : public TaskExecutorGetter，实现抽象类接口，提供接口：getExecutorLoad(cpu负载)、for_each(遍历所有线程)、addPoller(创建 EventPoller 线程池)。

### TaskQueue.h(由信号量控制的任务队列，加了线程锁，线程安全)
class TaskQueue，接口：push_task、push_exit、get_task、size。

### threadgroup.h(线程组管理，创建线程，移除线程)
class thread_group，成员：_threads(umap存储线程组)，接口：create_thread、remove_thread、is_thread_in、join_all、size。

### ThreadPool.h(线程池任务管理，管理线程组执行任务队列)
class ThreadPool : public TaskExecutor，成员：thread_group、TaskQueue<Task::Ptr>，接口：start(启动线程池)、async(异步加入任务到队列)。

### WorkThreadPool.h(创建一个工作线程池，可以加入线程负载均衡分配算法，类似EventPollerPool)
class WorkThreadPool : public TaskExecutorGetterImp，接口：getPoller、getFirstPoller、setPoolSize。


## Poller
### Pipe.h(管道对象封装)
class Pipe，成员：std::shared_ptr<PipeWrap>、EventPoller::Ptr _poller。

### PipeWrap.h(管道的封装，windows下由socket模拟)
class PipeWrap，成员：int _pipe_fd[2]，接口：write、read。

### SelectWrap.h(select 模型的简单封装)
class FdSet

### Timer.h(定时器对象)
class Timer，成员：EventPoller::Ptr(引用)，构造函数传参超时时长和超时回调函数，EventPoller 选传或自动获取，超时回调在 EventPoller 线程执行。

### EventPoller.h(基于epoll事件轮询模块)
class EventPoller : public TaskExecutor, public AnyStorage，基于epoll，可监听fd网络事件，async管道触发执行异步任务，doDelayTask定时器回调任务，runLoop执行事件循环体，添加/删除/修改监听事件，_event_map<网络fd和管道fd,回调>，_delay_task_map<延迟触发时间,回调>，_list_task<异步任务列表Task>。
class EventPollerPool :public TaskExecutorGetterImp，管理 EventPoller 线程池，可创建多个 EventPoller 线程，使用cpu负载均衡算法均匀分配线程，getPoller-》getExecutor 获得线程池内cpu负载最低的 EventPoller 线程。


## Network
### Buffer.h
class Buffer : public noncopyable，缓存抽象类，纯虚函数：data、size、toString、getCapacity，成员：ObjectStatistic<Buffer>对象个数统计。
class BufferOffset : public  Buffer，成员：typename _data，构造函数传参offset，data获取+offset偏移的buffer。
class BufferRaw : public Buffer，成员：char *_data，接口：setCapacity分配，assign赋值，指针式缓存，根据分配内存大小自动扩减容。
class BufferLikeString : public Buffer，成员：std::string _str，接口：erase、append、push_back、insert、assign、clear、capacity、reserve、resize、empty、substr等，字符串操作缓存。

### BufferSock.h
class BufferSock : public Buffer，成员：Buffer::Ptr _buffer、sockaddr_storage，管理_buffer指向的缓存。
class BufferList : public noncopyable，抽象类，接口：create、empty、count、send。
内部类
class BufferCallBack，成员：BufferList::SendResult回调函数，List<std::pair<Buffer::Ptr, bool> > 缓存列表，接口：sendFrontSuccess、sendCompleted，发送结果回调。
class BufferSendMsg final : public BufferList, public BufferCallBack，成员：_remain_size(剩余字节数)、_iovec(data和len组成的vector)、_iovec_off(_iovec当前发送下标)，接口：send、send_l(执行系统调用sendmsg)，socket发送数据时的buffer封装，用于tcp发送。
class BufferSendTo final: public BufferList, public BufferCallBack，接口：send(执行系统调用::sendto和::send)。
class BufferSendMMsg : public BufferList, public BufferCallBack，和 BufferSendMsg 类似，用于udp发送。

### Server.h
class SessionMap，成员：std::unordered_map<std::string, std::weak_ptr<Session> >，管理Session，add、del、get。
class SessionHelper，成员：Session::Ptr、SessionMap::Ptr、Server，记录session至全局的map，方便后面管理。
class Server : public mINI，成员：EventPoller::Ptr，初始化设置EventPoller线程。

### Session.h
class TcpSession : public Session
class UdpSession : public Session
class Session : public SocketHelper，成员：std::unique_ptr<toolkit::ObjectStatistic<toolkit::TcpSession> >、std::unique_ptr<toolkit::ObjectStatistic<toolkit::UdpSession> >，用于存储一对客户端与服务端间的关系。

### Socket.h
typedef enum ErrCode，自定义socket错误枚举。
class SockException : public std::exception，成员：ErrCode，错误信息类，用于抛出系统和自定义异常，接口：what、getErrCode、getCustomCode、reset。
typedef enum SockType，socket类型，udp、tcp、tcpserver。
class SockNum，成员：int _fd、SockType _type，析构时关闭socket。
class SockFD : public noncopyable，成员：SockNum、EventPoller，文件描述符fd的封装，析构时停止事件监听，关闭socket。
class MutexWrapper，接口：lock、unlock，线程锁的封装，默认使用递归锁recursive_mutex。
class SockInfo，抽象类，接口：get_local_ip、get_local_port、get_peer_ip、get_peer_port、getIdentifier。
class Socket : public noncopyable, public SockInfo，成员：SockFD、EventPoller(网络事件触发和异步执行在此线程)，异步IO Socket对象，包括tcp客户端、服务器和udp套接字，包含：错误回调、接收数据回调、tcp服务监听进入回调，connect、listen、send等接口的封装。
class SockSender，抽象类，接口：send、shutdown，重载运算符<<发送数据，定义socket发送接口。
class SocketHelper : public SockSender, public SockInfo, public TaskExecutorInterface，抽象类，成员：Socket、EventPoller，主要是对Socket类的二次封装，自定义类继承该类，实现纯虚接口即可创建一个完整的socket类，比如tcpclient。
class SockUtil，套接字工具类，封装了socket、网络的一些基本操作，提供静态全局接口，比如connect、listen等。
class TcpClient : public SocketHelper，抽象类，Tcp客户端，自定义类继承与该类，实现onConnect、onManager回调即可创建一个可运行的tcp客户端。
class TcpServer : public Server，可配置的TCP服务器。
class UdpServer : public Server，可配置的UDP服务器。

### ResourcePool.h(资源池)

