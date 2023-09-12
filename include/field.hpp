#pragma once

#include <any>
#include <string>
#include <optional>
#include <utility>

#include "type.hpp"
#include "type_union.hpp"

namespace Refl {
struct FieldBase {
    [[nodiscard]] std::string getName() const { return m_name_; }
    [[nodiscard]] virtual std::string getTypeName() const = 0;
    [[nodiscard]] virtual const TypeBase& getTypeInfo() const = 0;
    [[nodiscard]] bool isStaticMember() const {
        return m_is_static_member_;
    }

    virtual bool setObjValue(void* obj, void* value) = 0;
    virtual void* getObjValue(void* obj) const = 0;

    template<typename T>
    std::optional<T> getObjValueAs(void * obj) {
        auto value = getObjValue(obj);
        if (value == nullptr) {
            return std::nullopt;
        }

        return std::make_optional<T>(*(static_cast<T*>(value)));
    }

  protected:
    std::string m_name_;
    bool m_is_static_member_{false};
};

template<typename cls>
struct Field: public FieldBase {
    virtual bool setValueAny(cls* obj, std::any value) = 0;
    virtual bool setValue(cls* obj, void* value) = 0;
    virtual void* getValue(cls* obj) const = 0;
    virtual std::any getValueAny(cls* obj) const = 0;

    template<typename T>
    bool setFieldValue(cls* obj, T value) {
        return setValue(obj, &value);
    }

    template<typename T>
    std::optional<T> getValueAs(cls* obj) {
        auto value = getValue(obj);
        if (value == nullptr) {
            return std::nullopt;
        }

        return std::make_optional<T>(*(static_cast<T*>(value)));
    }

    bool setObjValue(void* obj, void* value) override {
        auto type_obj = static_cast<cls*>(obj);
        return setValue(type_obj, value);
    }

    void* getObjValue(void* obj) const override {
        auto type_obj = static_cast<cls*>(obj);
        return getValue(type_obj);
    }
};

template<typename cls, typename type, bool is_static_member = false>
struct TypeField: public Field<cls> {
    TypeField(const std::string& name, type cls::* p) {
        this->m_name_ = name;
        this->m_field_pointer_.first = p;
        this->m_is_static_member_ = is_static_member;
    }

    TypeField(const std::string& name, type* p) {
        this->m_name_ = name;
        this->m_field_pointer_.second = p;
        this->m_is_static_member_ = is_static_member;
    }

    bool setValueAny(cls* obj, std::any value) override {
        if (!value.has_value()) {
            return false;
        }

        return realSetValueAny(obj, value);
    }

    bool setValue(cls* obj, void* value) override {
        if (value == nullptr) {
            return false;
        }

        return realSetValue(obj, value);
    }

    std::any getValueAny(cls* obj) const override {
        auto value = realGetValueAny(obj);
        if (value.has_value()) {
            return value.value();
        }
        return {};
    }

    void* getValue(cls* obj) const override {
        return realGetValue(obj);
    }

    [[nodiscard]] std::string getTypeName() const override {
        return TypeBase::getType<type>().getName();
    }

    [[nodiscard]] const TypeBase& getTypeInfo() const override {
        return TypeBase::getType<type>();
    }

  private:
    template<bool U = is_static_member>
    typename std::enable_if<U, bool>::type realSetValueAny(cls* obj, std::any value) {
        auto type_value = std::any_cast<type>(value);
        (*(m_field_pointer_.second)) = type_value;
        return true;
    }

    template<bool U = is_static_member>
    typename std::enable_if<!U, bool>::type realSetValueAny(cls* obj, std::any value) {
        if (obj == nullptr) {
            return false;
        }
        auto type_value = std::any_cast<type>(value);
        obj->*(m_field_pointer_.first) = type_value;
        return true;
    }

    template<bool U = is_static_member>
    typename std::enable_if<U, bool>::type realSetValue(cls* obj, void* value) {
        auto type_pointer = static_cast<type*>(value);
        (*(m_field_pointer_.second)) = *type_pointer;
        return true;
    }

    template<bool U = is_static_member>
    typename std::enable_if<!U, bool>::type realSetValue(cls* obj, void* value) {
        if (obj == nullptr) {
            return false;
        }
        auto type_pointer = static_cast<type*>(value);
        obj->*(m_field_pointer_.first) = *type_pointer;
        return true;
    }

    template<bool U = is_static_member>
    std::optional<typename std::enable_if<U, type>::type> realGetValueAny(cls* obj) const {
        return std::make_optional(*(m_field_pointer_.second));
    }

    template<bool U = is_static_member>
    std::optional<typename std::enable_if<!U, type>::type> realGetValueAny(cls* obj) const {
        if (obj == nullptr) {
            return std::nullopt;
        }
        return std::make_optional((obj->*(m_field_pointer_.first)));
    }

    template<bool U = is_static_member>
    typename std::enable_if<U, void>::type* realGetValue(cls* obj) const {
        return m_field_pointer_.second;
    }

    template<bool U = is_static_member>
    typename std::enable_if<!U, void>::type* realGetValue(cls* obj) const {
        if (obj == nullptr) {
            return nullptr;
        }
        return &(obj->*(m_field_pointer_.first));
    }

    TypeUnion<type cls::*, type*> m_field_pointer_;
};
}  // namespace Refl