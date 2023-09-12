#include <iostream>
#include <string>

#include "field.hpp"
#include "refl_class.hpp"
#include "class_builder.hpp"

struct A {
    int a;
    static int b;
    float c;
    std::string d;

    A() {
        a = 100;
        b = 20;
        c = 35.f;
        d = "d";
    }

    A(int a, float c, std::string d) {
        this->a = a;
        this->c = c;
        this->d = std::move(d);
    }

    int func1(int h) const {
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
    std::cout << "hello world!" << std::endl;
    Refl::ClassBuilder<A>()
        .registerField("a", &A::a)
        .registerField("b", &A::b)
        .registerField("c", &A::c)
        .registerField("d", &A::d)
        .registerConstructor<>()
        .registerConstructor<int, float, std::string>()
        .registerMethod("func1", &A::func1)
        .registerMethod("addAndRet", &A::addAndRet)
        .registerStaticMethod("func2", &A::func2);
      
    A test{};
    test.a = 15;
    A::b = 25;
    test.c = 100.5f;
    test.d = "test";

    auto fields = Refl::ReflClass::getClass(test).getFields();
    std::cout << "field size: " << fields.size() << std::endl;
    for (const auto& field : fields) {
        auto strong_field = field.lock();
        if (strong_field == nullptr) {
            continue;
        }
        auto name = strong_field->getName();
        const auto& type = strong_field->getTypeInfo();
        if (type == Refl::TypeBase::getType<int>()) {
            std::cout << "name: " << name << " value: " << 
                    strong_field->getValueAs<int>(&test).value() << std::endl;
        } else if (type == Refl::TypeBase::getType<float>()) {
            std::cout << "name: " << name << " value: " << 
            strong_field->getValueAs<float>(&test).value() << std::endl;
        } else if (type == Refl::TypeBase::getType<std::string>()) {
            std::cout << "name: " << name << " value: " << 
            strong_field->getValueAs<std::string>(&test).value() << std::endl;
        } else {
            std::cout << "name: " << name << " value: unknown" << std::endl;
        }
    }

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

    auto empty_constructor_opt = Refl::ReflClass::getClass<A>().getConstructor<>();
    if (empty_constructor_opt.has_value()) {
        auto strong_empty_constructor = empty_constructor_opt.value().lock();
        if (strong_empty_constructor != nullptr) {
            auto obj = strong_empty_constructor->instance();
            std::cout << "A { a: " << obj.a << ", b: " << A::b << ", c: " << obj.c << ", d: " <<
                obj.d << " }" << std::endl;
        }
    }

    auto constructor_opt = Refl::ReflClass::getClass<A>().getConstructor<int, float, std::string>();
    if (constructor_opt.has_value()) {
        auto strong_constructor = constructor_opt.value().lock();
        if (strong_constructor != nullptr) {
            auto obj = strong_constructor->instance<int, float, std::string>(11, 3.f, "dTest");
            std::cout << "A { a: " << obj.a << ", b: " << A::b << ", c: " << obj.c << ", d: " <<
                      obj.d << " }" << std::endl;
        }
    }
    return 0;
}
