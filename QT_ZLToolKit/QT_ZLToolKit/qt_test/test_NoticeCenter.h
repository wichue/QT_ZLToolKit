#ifndef TEST_NOTICECENTER_H
#define TEST_NOTICECENTER_H

#include <csignal>
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/NoticeCenter.h"
using namespace std;
using namespace toolkit;

/**
 * @brief 广播中心，可以在程序的任意线程添加监听事件并定义回调；可以在任意线程发出一个事件，通知所有监听了该事件的地方执行回调。
 * 每个事件创建一个分发器 EventDispatcher ，分发器存放监听该事件的key和回调，加线程锁，多线程安全。
 * NoticeCenter::Instance() 定义对外接口，是全局单例对象，加线程锁，添加/删除事件、发出事件均是多线程安全。
 * addListener(指针key，事件名，回调) 可以在任意线程添加监听，emitEvent(事件名，参数列表) 可以在任意线程发出一个事件，注意：监听回调是在 emitEvent 所在线程执行的。
 *
 * class EventDispatcher,成员：std::unordered_multimap<void *, Any>（first指针，多个对象监听相同事件传的指针必须不同，second是监听该事件的回调），recursive_mutex，事件分发器，监听同一个事件的回调。
 * class NoticeCenter,成员：std::unordered_map<std::string, EventDispatcher::Ptr>（first事件名，second分发器），recursive_mutex，接口：emitEvent，addListener，delListener，全局单例。
 *
 * 下面实验：多线程监听相同事件，线程安全。
 */

//定义两个事件，事件是字符串类型
//广播名称1
#define NOTICE_NAME1 "NOTICE_NAME1"
//广播名称2
#define NOTICE_NAME2 "NOTICE_NAME2"

//程序退出标记
bool g_bExitFlag = false;

static void *tag1;
static void *tag2;

void* func0(void*) {
    //addListener方法第一个参数是标签，用来删除监听时使用
    //需要注意的是监听回调的参数列表个数类型需要与emitEvent广播时的完全一致，否则会有无法预知的错误
    NoticeCenter::Instance().addListener(tag1,NOTICE_NAME1,
            [](int &a,const char * &b,double &c,string &d){
        printf("func0=%d\n",a);
    });

    return nullptr;
}

void* func1(void*) {
    NoticeCenter::Instance().addListener(tag2,NOTICE_NAME1,
            [](int &a,const char * &b,double &c,string &d){
        printf("func1=%d\n",a);
    });
    return nullptr;
}

void* func2(void*) {
    //监听NOTICE_NAME2事件
    NoticeCenter::Instance().addListener(0,NOTICE_NAME2,
            [](string &d,double &c,const char *&b,int &a){
        printf("func2=%d\n",a);
    });

    return nullptr;
}

int test_NoticeCenter() {
    //设置程序退出信号处理函数
    signal(SIGINT, [](int){g_bExitFlag = true;});
    //设置日志
    Logger::Instance().add(std::make_shared<ConsoleChannel>());

    pthread_t tid[5];
    pthread_create(&tid[0],nullptr,func0,nullptr);
    pthread_create(&tid[1],nullptr,func1,nullptr);
    pthread_create(&tid[2],nullptr,func2,nullptr);

    int a = 0;
    while(!g_bExitFlag){
        const char *b = "b";
        double c = 3.14;
        string d("d");
        //每隔1秒广播一次事件，如果无法确定参数类型，可加强制转换
        NoticeCenter::Instance().emitEvent(NOTICE_NAME1,++a,(const char *)"b",c,d);
        NoticeCenter::Instance().emitEvent(NOTICE_NAME2,d,c,b,a);
        sleep(1); // sleep 1 second
    }
    return 0;
}

#endif // TEST_NOTICECENTER_H
