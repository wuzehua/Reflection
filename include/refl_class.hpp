#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "field.hpp"

namespace Refl {
struct ReflClassBase {
    virtual void* newInstance() = 0;
};

template<typename cls>
struct TypeReflClass: public ReflClassBase {

    static TypeReflClass& instance() {
        static TypeReflClass refl_class;
        return refl_class;
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

namespace ReflClass {
template<typename cls>
TypeReflClass<cls>& getClass() {
    return TypeReflClass<cls>::instance();
}

template<typename cls>
TypeReflClass<cls>& getClass(const cls& obj) {
    return getClass<cls>();
}

template<typename cls>
TypeReflClass<cls>& getClass(cls* obj) {
    return getClass<cls>();
}
}  // namespace ReflClass

}  // namespace Refl  