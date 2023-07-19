#include <iostream>
#include <string>

#include "../include/field.hpp"
#include "../include/refl_class.hpp"

struct A {
    int a;
    static int b;
    float c;
    std::string d;
};

int A::b = 10;

int main(int argc, char** argv)
{
    std::cout << "hello world!" << std::endl;
    Refl::ReflClass::getClass<A>()
        .registerField("a", &A::a)
        .registerField("b", &A::b)
        .registerField("c", &A::c)
        .registerField("d", &A::d);    
    A test{};
    test.a = 10;
    test.b = 20;
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
        auto value = strong_field->getValue(&test);
        const auto& type = strong_field->getTypeInfo();
        if (type == typeid(int)) {
            auto type_pointer = static_cast<int*>(value);
            std::cout << "name: " << name << " value: " << *type_pointer << std::endl;
        } else if (type == typeid(float)) {
            auto type_pointer = static_cast<float*>(value);
            std::cout << "name: " << name << " value: " << *type_pointer << std::endl;
        } else if (type == typeid(std::string)) {
            auto type_pointer = static_cast<std::string*>(value);
            std::cout << "name: " << name << " value: " << *type_pointer << std::endl;
        } else {
            std::cout << "name: " << name << " value: unknown" << std::endl;
        }
    }
    return 0;
}
