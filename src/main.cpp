#include <iostream>

#include "../include/field.hpp"
#include "../include/refl_class.hpp"

struct A {
    int a;
    static int b;
};

int A::b = 10;

int main(int argc, char** argv)
{
    std::cout << "hello world!" << std::endl;
    A test{};
    test.a = 10;
    std::cout << test.a << std::endl;
    Refl::TypeField<A, int, false> field{ "a", &A::a};
    field.setFieldValue(&test, 20);
    std::cout << test.a << "," << field.getTypeName() << std::endl;
    A::b = 10;
    std::cout << test.b << std::endl;
    Refl::TypeField<A, int, true> fieldB{ "b", &A::b};
    fieldB.setFieldValue(nullptr, 100);
    std::cout << test.b << "," << fieldB.getTypeName() << std::endl;
    auto classA = Refl::ReflClass::getClass(test);
    return 0;
}
