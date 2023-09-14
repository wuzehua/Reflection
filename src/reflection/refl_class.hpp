#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "field.hpp"
#include "method.hpp"
#include "constructor.hpp"

namespace Refl {
struct ReflClassBase {
    template<class... args>
    std::any newInstance(args&&... constructor_args) {
        auto constructor = findConstructor(std::vector<TypeBase*>({
            TypeBase::getTypePtr<args>()...
        }));

        if (constructor == nullptr) {
            return {};
        }

        return constructor->instanceObj(ArgUtils::makeArgs(std::forward<args>(constructor_args)...));
    }

    template<class... args>
    std::shared_ptr<void> newInstancePtr(args&&... constructor_args) {
        auto constructor = findConstructor(std::vector<TypeBase*>({
            TypeBase::getTypePtr<args>()...
        }));

        if (constructor == nullptr) {
            return nullptr;
        }

        return constructor->instanceObjPtr(ArgUtils::makeArgs(std::forward<args>(constructor_args)...));
    }
  protected:
    virtual std::shared_ptr<ConstructorBase> findConstructor(std::vector<TypeBase*>&& type_list) = 0;
};

template<typename cls>
struct TypeReflClass: public ReflClassBase {

    static TypeReflClass& instance() {
        static TypeReflClass refl_class;
        return refl_class;
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

    template<class... args>
    std::optional<std::weak_ptr<Constructor<cls>>> getConstructor() {
        for (const auto& constructor : m_constructor_list_) {
            if (constructor->template isMatchTemplate<args...>()) {
                return std::make_optional(std::weak_ptr<Constructor<cls>>(constructor));
            }
        }
        return std::nullopt;
    }

  protected:
    std::shared_ptr<ConstructorBase> findConstructor(std::vector<TypeBase*>&& type_list) override {
        for (auto constructor : m_constructor_list_) {
            if (constructor == nullptr) {
                continue;
            }

            if (constructor->isMatch(std::move(type_list))) {
                return constructor;
            }
        }
        return nullptr;
    }

  private:
    template<class cls_type = cls>
    friend class ClassBuilder;

    std::unordered_map<std::string, std::shared_ptr<Field<cls>>> m_field_map_;
    std::unordered_map<std::string, std::shared_ptr<Method<cls, false>>> m_method_map_;
    std::unordered_map<std::string, std::shared_ptr<Method<cls, true>>> m_static_method_map_;
    std::vector<std::shared_ptr<Constructor<cls>>> m_constructor_list_;
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