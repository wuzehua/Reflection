#pragma once

#include <string>
#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace Refl {
template<typename cls>
struct Field {
    std::string getName() const { return m_name_; }
    virtual bool setValue(cls* obj, void* value) = 0;
    virtual void* getValue(cls* obj) const = 0;
    virtual std::string getTypeName() const = 0;
    virtual const std::type_info& getTypeInfo() const = 0;

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

        auto type_pointer = static_cast<T*>(value);
        return std::make_optional<T>(*type_pointer);
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

    bool setValue(cls* obj, void* value) override {
        if (value == nullptr) {
            return false;
        }

        return realSetValue(obj, value);
    }

    void* getValue(cls* obj) const override {
        return realGetValue(obj);
    }

    std::string getTypeName() const override {
        return typeid(type).name();
    }

    const std::type_info& getTypeInfo() const override {
        return typeid(type);
    }

  private:
    template<bool U = is_static_member>
    typename std::enable_if<U, bool>::type realSetValue(cls* obj, void* value) {
        auto type_pointer = static_cast<type*>(value);
        (*m_static_field_pointer_) = *type_pointer;
        return true;
    }

    template<bool U = is_static_member>
    typename std::enable_if<!U, bool>::type realSetValue(cls* obj, void* value) {
        if (obj == nullptr) {
            return false;
        }
        auto type_pointer = static_cast<type*>(value);
        obj->*m_field_pointer_ = *type_pointer;
        return true;
    }

    template<bool U = is_static_member>
    typename std::enable_if<U, void>::type* realGetValue(cls* obj) const {
        return m_static_field_pointer_;
    }

    template<bool U = is_static_member>
    typename std::enable_if<!U, void>::type* realGetValue(cls* obj) const {
        if (obj == nullptr) {
            return nullptr;
        }
        return &(obj->*m_field_pointer_);
    }

    type cls::* m_field_pointer_;
    type* m_static_field_pointer_;
};
}  // namespace Refl