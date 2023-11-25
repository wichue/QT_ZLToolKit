#ifndef TEST_ANY_H
#define TEST_ANY_H


#include <csignal>
#include <iostream>
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/TimeTicker.h"
#include "Poller/Timer.h"

using namespace std;
using namespace toolkit;

class Student{
public:
    Student(int age):age(age) {
        printf("Student\n");
    }

    ~Student() {
        printf("~Student\n");
    }

    Student(Student &s) {
        printf("Student copy\n");
        age = s.age;
    }

    int age;
};

template <typename FUNC>
void test_func(FUNC &&func) {

}

/**
 * @brief Any :可以保存任意的对象
 */
void test_Any() {

    //1.Any保存类的对象
    {
        Any aa;
        //创建 Student 的智能指针，(17)是构造函数的参数
        aa.set<Student>(17);

        //拷贝构造，get<Student>(bool可选参数)返回智能指针所管理的原始指针的对象引用
        Student S1 = aa.get<Student>();
//        aa.reset();//如果天提前释放aa，则捕获异常，打印： ex=Any is empty
        try{
            printf("aa age=%d\n",aa.get<Student>().age);
        }catch(std::exception& e) {
            printf("ex=%s\n",e.what());
        }

        printf("s1 age=%d\n",S1.age);

        //离开作用域打印：
        // Student
        // Student copy
        // aa age=17
        // s1 age=17
        // ~Student
        // ~Student
    }

    //2.Any保存 function 函数指针模板
    Any bb;
    //set<function函数指针模板的数据类型>(function 的实例化，lambda表达式)
    bb.set<std::function<void(int)>>([](int a){
        printf("a=%d\n",a);
    });

    //获取bb所管理的对象并调用方法，(bool可选参数)(10：调用对象所传的参数)
    bb.get<std::function<void(int)>>()(10);//调用lambda表达式，打印：a=10


    //退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
}

#endif // TEST_ANY_H
