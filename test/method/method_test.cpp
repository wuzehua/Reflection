#include <iostream>
#include <string>

#include "reflection/refl_class.hpp"
#include "reflection/class_builder.hpp"

struct A {
    int a;
    static int b;
    float c;
    std::string d;

    [[nodiscard]] int func1(int h) const {
        std::cout << "func1: " << a + b + h << std::endl;
        return a * b * h;
    }

    int addAndRet(int h) {
        a += h;
        std::cout << "addAndRet: " << a << std::endl;
        return a;
    }

    static int func2(const std::string& i, int a) {
        std::cout << "func2: " << i << b * 2 + a << std::endl;
        return b + a;
    }
};

int A::b = 10;

int main(int argc, char** argv)
{
    std::cout << "Method test!" << std::endl;
    Refl::ClassBuilder<A>()
            .registerMethod("func1", &A::func1)
            .registerMethod("addAndRet", &A::addAndRet)
            .registerStaticMethod("func2", &A::func2);

    A test{};
    test.a = 15;
    A::b = 25;
    test.c = 100.5f;
    test.d = "test";

    auto func1 = Refl::ReflClass::getClass(test).getMethod("func1");
    auto strong_func1 = func1.value().lock();
    if (strong_func1 != nullptr) {
        auto ret = strong_func1->invokeWithRetType(&test, Refl::TypeUtils::getType<int>(), 2);
        if (ret.has_value()) {
            std::cout << "func1 result: " << ret.value() << std::endl;
        }
    }

    auto func2 = Refl::ReflClass::getClass(test).getStaticMethod("func2");
    auto strong_func2 = func2.value().lock();
    if (strong_func2 != nullptr) {
        std::string input = "input: ";
        auto ret = strong_func2->invoke<int, const std::string&, int>(&test, input, 1);
        if (ret.has_value()) {
            std::cout << "func2 result: " << ret.value() << std::endl;
        }
    }

    auto func_add_and_ret = Refl::ReflClass::getClass(test).getMethod("addAndRet");
    auto strong_func_add_and_ret = func_add_and_ret.value().lock();
    if (strong_func_add_and_ret != nullptr) {
        auto ret = strong_func_add_and_ret->invoke<int, int>(&test, 50);
        if (ret.has_value()) {
            std::cout << "addAndRet result: " << ret.value() << std::endl;
        }
    }

    return 0;
}
