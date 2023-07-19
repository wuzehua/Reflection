#pragma once

#include <string>
#include <unordered_map>

#include "field.hpp"

namespace Refl {
template<typename cls>    
struct TypeReflClass;

struct ReflClass {
    template<typename cls>
    static TypeReflClass<cls> getClass(const cls& obj);

    template<typename cls>
    static TypeReflClass<cls> getClass(cls* obj);

    template<typename cls>
    static TypeReflClass<cls> getClass();

    virtual void* newInstance() = 0;

};

template<typename cls>
struct TypeReflClass: public ReflClass {

    static TypeReflClass& instance() {
        static TypeReflClass reflClass;
        return reflClass;
    }

    void* newInstance() override {
        return newTypeInstance();
    }

    cls* newTypeInstance() {
        return nullptr;
    }
  private:
    std::unordered_map<std::string, Field<cls>> m_field_map_;
};

template<typename cls>
TypeReflClass<cls> ReflClass::getClass(const cls& obj) {
    return ReflClass::getClass<cls>();
}

template<typename cls>
TypeReflClass<cls> ReflClass::getClass(cls* obj) {
    return ReflClass::getClass<cls>();
}

template<typename cls>
TypeReflClass<cls> ReflClass::getClass() {
    return TypeReflClass<cls>::instance();
}

}  // namespace Refl  