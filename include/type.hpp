#pragma once

#include <string>

namespace Refl {
struct TypeBase {
    [[nodiscard]] virtual std::string getName() const = 0;
    [[nodiscard]] virtual uint64_t getId() const = 0;

    inline bool operator ==(const TypeBase& type) const {
        return getId() == type.getId();
    }

    template<typename T>
    static const TypeBase& getType();
};

template<typename type>
struct Type: public TypeBase {

    static Type& instance() {
        static Type type_instance;
        return type_instance;
    }

    [[nodiscard]] std::string getName() const override {
        return "";
    }

    [[nodiscard]] uint64_t getId() const override {
        return (uint64_t)(&instance());
    }
  private:
};

template<typename T>
const TypeBase& TypeBase::getType() {
    return Type<T>::instance();
}

namespace TypeUtils {
template<typename T>
const Type<T>& getType() {
    return Type<T>::instance();
}
}  // namespace TypeUtils
}  // namespace Refl