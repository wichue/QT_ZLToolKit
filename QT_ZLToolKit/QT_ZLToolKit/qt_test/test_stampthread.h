#ifndef TEST_STAMPTHREAD_H
#define TEST_STAMPTHREAD_H

#include "Util/util.h"
#include <sys/time.h>
using namespace std;
using namespace toolkit;

/**
 * 测试时间戳线程
 * 只要执行了静态方法initMillisecondThread，就会创建时间戳线程，最多创建1个，线程名称：stamp thread
 * 提供getCurrentMillisecond和getCurrentMicrosecond接口，获取程序启动到当前时间的毫秒数和微秒数
 * 或从1970年开始到当前时间的毫秒数和微秒数
 * 程序启动后有两个线程：QT_ZLToolKit(主线程)和stamp thread
 */
void test_stampthread() {

    uint64_t cur_ms = getCurrentMillisecond(true);
    printf("cur_ms = %lu\n",cur_ms);

    usleep(100*1000);

    cur_ms = getCurrentMillisecond(true);
    printf("cur_ms = %lu\n",cur_ms);
}

#endif // TEST_STAMPTHREAD_H
