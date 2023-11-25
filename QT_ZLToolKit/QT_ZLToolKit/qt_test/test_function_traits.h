#ifndef TEST_FUNCTION_TRAITS_H
#define TEST_FUNCTION_TRAITS_H

#include <csignal>
#include <iostream>
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/TimeTicker.h"
#include "Poller/Timer.h"
#include "Util/function_traits.h"

using namespace std;
using namespace toolkit;

/**
 * @brief 源码类似于： 《深入应用C++11 代码优化与工程级应用》3.3.6　function_traits
 * function_traits 用来获取所有函数语义类型的信息(函数类型、返回类型、参数个数和参数的具体类型)，通过 stl_function_type 把任意函数转换成 std::function。
 * 函数类型包括：普通函数、函数指针、function/lambda、成员函数、函数对象。
 *
 * 实现function_traits的关键技术：
 * 要通过模板特化和可变参数模板来获取函数类型和返回类型。
 *  先定义一个基本的function_traits的模板类：
 *     template<typename T>
 *     struct function_traits;
 *  再通过特化，将返回类型和可变参数模板作为模板参数，就可以获取函数类型、函数返回值和参数的个数了。
 *
 * 如:
 *  int func(int a, string b);
 * ## 获取函数类型
 *      function_traits<decltype(func)>::function_type; // int __cdecl(int, string)
 * # 获取函数返回值
 *      function_traits<decltype(func)>::return_type;   // int
 * # 获取函数的参数个数
 *      function_traits<decltype(func)>::arity;         // 2
 * # 获取函数第一个入参类型
 *      function_traits<decltype(func)>::args<0>::type; // int
 * # 获取函数第二个入参类型
 *      function_traits<decltype(func)>::args<1>::type; // string
 * # 将函数转换为 std::function
 *   stl_function_type
 */

//打印数据类型
template<typename T>
void printType()
{
    printf("%s\n",demangle(typeid(T).name()).c_str());
}

//自定义类
class Student2{

};

//函数指针
float(*cast_func)(int, int, int, int);

//普通函数
int func(int a, Student2 b)
{
    printf("a=%d\n",a);
    return 0;
}

struct AA
{
    int f(int a, int b)volatile { return a + b; }//成员函数
    int operator()(int)const { return 0; }//函数对象
};

//function 函数包装模板,指向 lambda 表达式
std::function<int(int)> func_lam = [](int a) {return a; };

template <typename FUNC>
void to_function(FUNC &&func) {
    using stl_func = typename function_traits<typename std::remove_reference<FUNC>::type>::stl_function_type;
    stl_func f = func;

    f(10,Student2());//调用func,打印： a=10
}

void test_function_traits() {

    //1.获取函数信息
    printf("func:%s\n",demangle(typeid(function_traits<decltype(func)>::function_type).name()).c_str());//打印函数类型
    printf("func ret:%s\n",demangle(typeid(function_traits<decltype(func)>::return_type).name()).c_str());//打印返回值类型
    printf("fucn arg num:%d\n",function_traits<decltype(func)>::arity);//打印参数个数
    printf("fucn arg[0]:%s\n",demangle(typeid(function_traits<decltype(func)>::args<0>::type).name()).c_str());//打印第一个参数的类型

    printType<function_traits<std::function<int(int)>>::function_type>();
    printType<function_traits<std::function<int(int)>>::args<0>::type>();
    printType<function_traits<decltype(func_lam)>::function_type>();

    printType<function_traits<decltype(cast_func)>::function_type>();
    printType<function_traits<AA>::function_type>();
    using T = decltype(&AA::f);
    printType<T>();
    printType<function_traits<decltype(&AA::f)>::function_type>();
    static_assert(std::is_same<function_traits<decltype(func_lam)>::return_type, int>::value, "");

    //2.使用 stl_function_type 把任意函数转换为 std::function
    to_function(func);

    /**
     * 打印：
     * func:int (int, Student2)
     * func ret:int
     * fucn arg num:2
     * fucn arg[0]:int
     * int (int)
     * int
     * int (int)
     * float (int, int, int, int)
     * int (int)
     * int (AA::*)(int, int) volatile
     * int (int, int)
     * a=10
     */

    //退出程序事件处理
    static semaphore sem;
    signal(SIGINT, [](int) { sem.post(); });// 设置退出信号
    sem.wait();
}

#endif // TEST_FUNCTION_TRAITS_H
