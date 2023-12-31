#include <iostream>
#include <string>

#include "reflection/field.hpp"
#include "reflection/refl_class.hpp"
#include "reflection/class_builder.hpp"

struct A {
    int a;
    static int b;
    float c;
    std::string d;
};

int A::b = 10;

int main(int argc, char** argv)
{
    std::cout << "Field test!" << std::endl;
    Refl::ClassBuilder<A>()
            .registerField("a", &A::a)
            .registerField("b", &A::b)
            .registerField("c", &A::c)
            .registerField("d", &A::d);

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

    return 0;
}
