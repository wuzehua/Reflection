#include <iostream>
#include <string>

#include "../include/field.hpp"
#include "../include/refl_class.hpp"

struct A {
    int a;
    static int b;
    float c;
    std::string d;

    void func1(int h) {
        std::cout << "func1: " << a + b + h << std::endl;
    }

    static void func2() {
        std::cout << "func2: " << b * 2 << std::endl;
    }
};

int A::b = 10;

int main(int argc, char** argv)
{
    std::cout << "hello world!" << std::endl;
    Refl::ReflClass::getClass<A>()
        .registerField("a", &A::a)
        .registerField("b", &A::b)
        .registerField("c", &A::c)
        .registerField("d", &A::d)
        .registerMethod("func1", &A::func1)
        .registerStaticMethod("func2", &A::func2);
      
    A test{};
    test.a = 15;
    test.b = 25;
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
        strong_func1->invoke<void, int>(&test, 2);
    }
    return 0;
}
