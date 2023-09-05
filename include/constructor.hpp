#pragma once

#include <utility>

#include "type.hpp"

namespace Refl {
template<typename>
struct ConstructorBase;

namespace ConstructorUtils {
template<typename cls, class... args>
cls invokeConstructor(ConstructorBase<cls>* constructor, args&&... constructor_args);
}  // namespace ConstructorUtils


template<typename cls>
struct ConstructorBase {
    template<class... args>
    cls instance(args&&... constructor_args) {
        return ConstructorUtils::invokeConstructor(this, std::forward<args>(constructor_args)...);
    }

    template<class... args>
    bool isMatch() const {
        return isMatch(std::vector<TypeBase*>({
            TypeBase::getTypePtr<args>()...
        }));
    }

    virtual bool isMatch(std::vector<TypeBase*>&& type_list) const = 0;
};

template<typename cls, class... args>
struct Constructor: public ConstructorBase<cls> {
    Constructor(): m_args_type_list_({
        TypeBase::getTypePtr<args>()...
    }) {
        m_constructor_wrapper_ = [](args&&... constructor_args) -> cls {
            return cls(std::forward<args>(constructor_args)...);
        };
    }

    cls realInvoke(args&&... constructor_args) {
        return m_constructor_wrapper_(std::forward<args>(constructor_args)...);
    }

    bool isMatch(std::vector<TypeBase*>&& type_list) const override {
        if (type_list.size() != m_args_type_list_.size()) {
            return false;
        }

        auto size = m_args_type_list_.size();
        for (auto i = 0; i < size; i++) {
            if (type_list[i] == nullptr || m_args_type_list_[i] == nullptr) {
                return false;
            }
            if (*type_list[i] != *m_args_type_list_[i]) {
                return false;
            }
        }
        return true;
    }
  private:
    std::function<cls(args...)> m_constructor_wrapper_;
    std::vector<TypeBase*> m_args_type_list_;
};

namespace ConstructorUtils {
template<typename cls, class... args>
cls invokeConstructor(ConstructorBase<cls>* constructor, args&&... constructor_args) {
    auto real_constructor = static_cast<Constructor<cls, args...>*>(constructor);
    return real_constructor->realInvoke(std::forward<args>(constructor_args)...);
}
}  // namespace ConstructorUtils
}  // namespace Refl