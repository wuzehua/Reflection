#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "field.hpp"
#include "method.hpp"

namespace Refl {
struct ReflClassBase {
    virtual std::any newInstance() = 0;
};

template<typename cls>
struct TypeReflClass: public ReflClassBase {

    static TypeReflClass& instance() {
        static TypeReflClass refl_class;
        return refl_class;
    }

    std::any newInstance() override {
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

    template<typename ret_type, class... args>
    TypeReflClass& registerMethod(const std::string& name, ret_type (cls::*p)(args...)) {
        this->m_method_map_[name] = std::make_shared<TypeMethod<cls, false, ret_type, args...>>(name, p);
        return *this;
    }

    template<typename ret_type, class... args>
    TypeReflClass& registerMethod(const std::string& name, ret_type (cls::*p)(args...) const) {
        this->m_method_map_[name] = std::make_shared<TypeMethod<cls, false, ret_type, args...>>(name, p);
        return *this;
    }

    template<typename ret_type, class... args>
    TypeReflClass& registerStaticMethod(const std::string& name, ret_type (*p)(args...)) {
        this->m_static_method_map_[name] = std::make_shared<TypeMethod<cls, true, ret_type, args...>>(name, p);
        return *this;
    }

    std::weak_ptr<Field<cls>> getField(const std::string& name) {
        if (this->m_field_map_.count(name) == 0) {
            return nullptr;
        }

        return std::weak_ptr<Field<cls>>(this->m_field_map_[name]);
    }

    std::optional<std::weak_ptr<Method<cls, false>>> getMethod(const std::string& name) {
        if (this->m_method_map_.count(name) == 0) {
            return std::nullopt;
        }

        return std::make_optional(std::weak_ptr<Method<cls, false>>(this->m_method_map_[name]));
    }

    std::optional<std::weak_ptr<Method<cls, true>>> getStaticMethod(const std::string& name) {
        if (this->m_static_method_map_.count(name) == 0) {
            return std::nullopt;
        }

        return std::make_optional(std::weak_ptr<Method<cls, true>>(this->m_static_method_map_[name]));
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
    std::unordered_map<std::string, std::shared_ptr<Method<cls, false>>> m_method_map_;
    std::unordered_map<std::string, std::shared_ptr<Method<cls, true>>> m_static_method_map_;
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