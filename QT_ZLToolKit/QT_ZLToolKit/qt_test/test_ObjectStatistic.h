#ifndef TEST_OBJECTSTATISTIC_H
#define TEST_OBJECTSTATISTIC_H

#include <csignal>
#include <iostream>
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/TimeTicker.h"
#include "Poller/Timer.h"

using namespace std;
using namespace toolkit;

class Test_CLS {

public:
    int get_count() {
        return _statistic.count();
    }
private:
    ObjectStatistic<Test_CLS> _statistic;
};

StatisticImp(Test_CLS)

/**
 * @brief ObjectStatistic ：统计类所实例化对象的个数
 * 创建私有成员： ObjectStatistic<Test_Obj> _statistic;并使用宏声明： StatisticImp(Test_Obj)。
 * 在任意实例化的对象里均可调用接口 _statistic.count 查询实例化对象的总数。
 */
void test_ObjectStatistic() {
    Test_CLS tTest_CLS1;
    Test_CLS tTest_CLS2;

    printf("count=%d\n",tTest_CLS1.get_count());//count=2

    {
        Test_CLS tTest_CLS3;
        printf("count=%d\n",tTest_CLS1.get_count());//count=3
    }

    printf("count=%d\n",tTest_CLS1.get_count());//count=2

    //退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
}

#endif // TEST_OBJECTSTATISTIC_H
