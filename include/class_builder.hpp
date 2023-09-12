#pragma once

#include "refl_class.hpp"

namespace Refl {
template<class cls>
struct ClassBuilder {
    ClassBuilder(): m_refl_class_(TypeReflClass<cls>::instance()) {}

    template<typename T>
    ClassBuilder& registerField(const std::string& name, T cls::* p) {
        this->m_refl_class_.m_field_map_[name] = std::make_shared<TypeField<cls, T, false>>(name, p);
        return *this;
    }

    template<typename T>
    ClassBuilder& registerField(const std::string& name, T* p) {
        this->m_refl_class_.m_field_map_[name] = std::make_shared<TypeField<cls, T, true>>(name, p);
        return *this;
    }

    template<typename ret_type, class... args>
    ClassBuilder& registerMethod(const std::string& name, ret_type (cls::*p)(args...)) {
        this->m_refl_class_.m_method_map_[name] = std::make_shared<TypeMethod<cls, false, ret_type, args...>>(name, p);
        return *this;
    }

    template<typename ret_type, class... args>
    ClassBuilder& registerMethod(const std::string& name, ret_type (cls::*p)(args...) const) {
        this->m_refl_class_.m_method_map_[name] = std::make_shared<TypeMethod<cls, false, ret_type, args...>>(name, p);
        return *this;
    }

    template<typename ret_type, class... args>
    ClassBuilder& registerStaticMethod(const std::string& name, ret_type (*p)(args...)) {
        this->m_refl_class_.m_static_method_map_[name] = std::make_shared<TypeMethod<cls, true, ret_type, args...>>(name, p);
        return *this;
    }

    template<class... args>
    ClassBuilder& registerConstructor() {
        auto constructor = std::make_shared<TypeConstructor<cls, args...>>();
        this->m_refl_class_.m_constructor_list_.emplace_back(constructor);
        return *this;
    }
  private:
    TypeReflClass<cls>& m_refl_class_;
};
}  // namespace Refl
