#include <iostream>
#include <string>

#include "reflection/refl_class.hpp"
#include "reflection/class_builder.hpp"

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
};

int A::b = 10;

int main(int argc, char** argv)
{
    std::cout << "Constructor test!" << std::endl;
    Refl::ClassBuilder<A>()
            .registerConstructor<>()
            .registerConstructor<int, float, std::string>();

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
