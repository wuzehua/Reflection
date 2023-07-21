#pragma once

#include <any>
#include <string>
#include <optional>
#include <utility>

#include "type.hpp"

namespace Refl {
template<typename cls>
struct Field {
    std::string getName() const { return m_name_; }
    virtual bool setValue(cls* obj, std::any value) = 0;
    virtual std::any getValue(cls* obj) const = 0;
    virtual std::string getTypeName() const = 0;
    virtual const TypeBase& getTypeInfo() const = 0;

    template<typename T>
    bool setFieldValue(cls* obj, T value) {
        return setValue(obj, &value);
    }

    template<typename T>
    std::optional<T> getValueAs(cls* obj) {
        auto value = getValue(obj);
        if (!value.has_value()) {
            return std::nullopt;
        }

        return std::make_optional<T>(std::any_cast<T>(value));
    }

    bool isStaticMember() {
        return m_is_static_memeber_;
    }

  protected:
    std::string m_name_;
    bool m_is_static_memeber_{false}; 
};

template<typename cls, typename type, bool is_static_member = false>
struct TypeField: public Field<cls> {
    TypeField(const std::string& name, type cls::* p) {
        this->m_name_ = name;
        this->m_field_pointer_ = p;
        this->m_is_static_memeber_ = is_static_member;
    }

    TypeField(const std::string& name, type* p) {
        this->m_name_ = name;
        this->m_static_field_pointer_ = p;
        this->m_is_static_memeber_ = is_static_member;
    }

    bool setValue(cls* obj, std::any value) override {
        if (!value.has_value()) {
            return false;
        }

        return realSetValue(obj, value);
    }

    std::any getValue(cls* obj) const override {
        auto value = realGetValue(obj);
        if (value.has_value()) {
            return value.value();
        }
        return {};
    }

    std::string getTypeName() const override {
        return TypeBase::getType<type>().getName();
    }

    const TypeBase& getTypeInfo() const override {
        return TypeBase::getType<type>();
    }

  private:
    template<bool U = is_static_member>
    typename std::enable_if<U, bool>::type realSetValue(cls* obj, std::any value) {
        auto type_value = std::any_cast<type>(value);
        (*m_static_field_pointer_) = type_value;
        return true;
    }

    template<bool U = is_static_member>
    typename std::enable_if<!U, bool>::type realSetValue(cls* obj, std::any value) {
        if (obj == nullptr) {
            return false;
        }
        auto type_value = std::any_cast<type>(value);
        obj->*m_field_pointer_ = type_value;
        return true;
    }

    template<bool U = is_static_member>
    std::optional<typename std::enable_if<U, type>::type> realGetValue(cls* obj) const {
        return std::make_optional(*m_static_field_pointer_);
    }

    template<bool U = is_static_member>
    std::optional<typename std::enable_if<!U, type>::type> realGetValue(cls* obj) const {
        if (obj == nullptr) {
            return std::nullopt;
        }
        return std::make_optional((obj->*m_field_pointer_));
    }

    type cls::* m_field_pointer_;
    type* m_static_field_pointer_;
};
}  // namespace Refl