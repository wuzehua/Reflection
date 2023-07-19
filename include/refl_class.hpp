#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "field.hpp"

namespace Refl {
template<typename cls>    
struct TypeReflClass;

struct ReflClass {
    template<typename cls>
    static TypeReflClass<cls>& getClass(const cls& obj);

    template<typename cls>
    static TypeReflClass<cls>& getClass(cls* obj);

    template<typename cls>
    static TypeReflClass<cls>& getClass();

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

    template<typename T>
    TypeReflClass& registerField(const std::string& name, T cls::* p) {
        this->m_field_map_[name] = std::make_shared<TypeField<cls, T, false>>(name, p);
        return *this;
    }

    template<typename T>
    TypeReflClass& registerField(const std::string& name, T* p) {
        this->m_field_map_[name] = std::make_shared<TypeField<cls, T, true>>(name, p);
        return *this;
    }

    std::weak_ptr<Field<cls>> getField(const std::string& name) {
        if (this->m_field_map_.count(name) == 0) {
            return nullptr;
        }

        return std::weak_ptr<Field<cls>>(this->m_field_map_[name]);
    }

    std::vector<std::weak_ptr<Field<cls>>> getFields() {
        auto size = this->m_field_map_.size();
        std::vector<std::weak_ptr<Field<cls>>> fields(size);
        int index = 0;
        for (const auto& [key, value] : this->m_field_map_) {
            fields[index] = std::weak_ptr<Field<cls>>(value);
            index++;
        }
        return fields;
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<Field<cls>>> m_field_map_;
};

template<typename cls>
TypeReflClass<cls>& ReflClass::getClass(const cls& obj) {
    return ReflClass::getClass<cls>();
}

template<typename cls>
TypeReflClass<cls>& ReflClass::getClass(cls* obj) {
    return ReflClass::getClass<cls>();
}

template<typename cls>
TypeReflClass<cls>& ReflClass::getClass() {
    return TypeReflClass<cls>::instance();
}

}  // namespace Refl  