#ifndef TEST_RESOURCEPOOL_H
#define TEST_RESOURCEPOOL_H

#include <csignal>
#include <iostream>
#include <random>
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/ResourcePool.h"
#include "Thread/threadgroup.h"
#include <list>

using namespace std;
using namespace toolkit;

//程序退出标志
bool g_bExitFlag = false;

/**
* @brief ResourcePool ：基于智能指针实现的一个循环池，不需要手动回收对象
* ResourcePool<C> 是个类模板，可传入自定义数据类型C，内存池中存放的是该数据类型C的指针数组 std::vector<C *> _objs。
* setSize(_pool_size) 可设置内存池最大容量，当超出最大容量，recycle 不再回收该资源，而是直接释放。
* obtain 获取内存，返回的是指向内存对象C的自定义智能指针 shared_ptr_imp<C>，特点是提供接口quit，当内存对象使用完后，可以选择不再回收到内存池，此时可以自定义回收或直接释放。
* obtain2 获取内存，返回指向内存对象C的智能指针 std::shared_ptr<C>，当智能指针离开作用域时自动回收内存对象C到内存池。
* 私有接口：
* getPtr 获取C原始指针，当 _objs 为空时分配一个新的C*返回，当 _objs 不为空则从 _objs 尾部取已一个C*，并从 _objs 中删除。
* recycle 回收C*，如果 _objs 已满(>=_pool_size)，直接释放C*，否则回收到 _objs 尾部。
*
* 总结：内存池中存放的是任意类型数据指针C*，C大小固定或可动态扩容，刚开始内存池是空的，使用时分配内存，用完后回收到内存池，下次再使用时就不用重新分配了，直接用上次分配并回收的C*；
* 不用担心高并发内存池不够用，因为当内存池为空时总会立即分配内存，如果分配的太多，回收时超出内存池大小后会直接释放，合理的内存池大小在高并发时会减少分配和释放的次数。
*/
class string_imp : public string{
public:
    template<typename ...ArgTypes>
    string_imp(ArgTypes &&...args) : string(std::forward<ArgTypes>(args)...){
        DebugL << "创建string对象:" << this << " " << *this;
    };
    ~string_imp(){
        WarnL << "销毁string对象:" << this << " " << *this;
    }
};


//后台线程任务
void onRun(ResourcePool<string_imp> &pool,int threadNum){
    std::random_device rd;
    while(!g_bExitFlag){
        //从循环池获取一个可用的对象
        auto obj_ptr = pool.obtain();
        if(obj_ptr->empty()){
            //这个对象是全新未使用的
            InfoL << "后台线程 " << threadNum << ":" << "obtain a emptry object!";
        }else{
            //这个对象是循环使用的
            InfoL << "后台线程 " << threadNum << ":" << *obj_ptr;
        }
        //标记该对象被本线程使用
        obj_ptr->assign(StrPrinter << "keeped by thread:" << threadNum );

        //随机休眠，打乱循环使用顺序
        usleep( 1000 * (rd()% 10));
        obj_ptr.reset();//手动释放，也可以注释这句代码。根据RAII的原理，该对象会被自动释放并重新进入循环列队
        usleep( 1000 * (rd()% 1000));
    }
}

int test_ResourcePool() {
    //初始化日志
    Logger::Instance().add(std::make_shared<ConsoleChannel>());
    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    //大小为50的循环池
    ResourcePool<string_imp> pool;
    pool.setSize(50);

    //获取一个对象,该对象将被主线程持有，并且不会被后台线程获取并赋值
    auto reservedObj = pool.obtain();
    //在主线程赋值该对象
    reservedObj->assign("This is a reserved object , and will never be used!");

    thread_group group;
    //创建4个后台线程，该4个线程模拟循环池的使用场景，
    //理论上4个线程在同一时间最多同时总共占用4个对象


    WarnL << "主线程打印:" << "开始测试，主线程已经获取到的对象应该不会被后台线程获取到:" << *reservedObj;

    for(int i = 0 ;i < 4 ; ++i){
        group.create_thread([i,&pool](){
            onRun(pool,i);
        });
    }

    //等待3秒钟，此时循环池里面可用的对象基本上最少都被使用过一遍了
    sleep(3);

    //但是由于reservedObj早已被主线程持有，后台线程是获取不到该对象的
    //所以其值应该尚未被覆盖
    WarnL << "主线程打印: 该对象还在被主线程持有，其值应该保持不变:" << *reservedObj;

    //获取该对象的引用
    auto &objref = *reservedObj;

    //显式释放对象,让对象重新进入循环列队，这时该对象应该会被后台线程持有并赋值
    reservedObj.reset();

    WarnL << "主线程打印: 已经释放该对象,它应该会被后台线程获取到并被覆盖值";

    //再休眠3秒，让reservedObj被后台线程循环使用
    sleep(3);

    //这时，reservedObj还在循环池内，引用应该还是有效的，但是值应该被覆盖了
    WarnL << "主线程打印:对象已被后台线程赋值为:" << objref << endl;

    {
        WarnL << "主线程打印:开始测试主动放弃循环使用功能";

        List<decltype(pool)::ValuePtr> objlist;
        for (int i = 0; i < 8; ++i) {
            reservedObj = pool.obtain();
            string str = StrPrinter << i << " " << (i % 2 == 0 ? "此对象将脱离循环池管理" : "此对象将回到循环池");
            reservedObj->assign(str);
            reservedObj.quit(i % 2 == 0);
            objlist.emplace_back(reservedObj);
        }
    }
    sleep(3);

    //通知后台线程退出
    g_bExitFlag = true;
    //等待后台线程退出
    group.join_all();
    return 0;
}












#endif // TEST_RESOURCEPOOL_H
