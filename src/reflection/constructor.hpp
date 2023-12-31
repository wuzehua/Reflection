#pragma once

#include <utility>

#include "type.hpp"
#include "args.hpp"

namespace Refl {
struct ConstructorBase {
    template<class... args>
    [[nodiscard]] bool isMatchTemplate() const {
        return isMatch(std::vector<TypeBase*>({
           TypeBase::getTypePtr<args>()...
        }));
    }

    virtual bool isMatch(std::vector<TypeBase*>&& type_list) const = 0;
    virtual std::any instanceObj(const Args& args) = 0;
    virtual std::shared_ptr<void> instanceObjPtr(const Args& args) = 0;
};

template<typename cls>
struct Constructor: public ConstructorBase {
    template<class... args>
    cls instance(args&&... constructor_args) {
        return realInstance(ArgUtils::makeArgs(std::forward<args>(constructor_args)...));
    }

    template<class... args>
    std::shared_ptr<cls> instancePtr(args&&... constructor_args) {
        return realInstancePtr(ArgUtils::makeArgs(std::forward<args>(constructor_args)...));
    }

 protected:
    virtual cls realInstance(const Args& args) = 0;
    virtual std::shared_ptr<cls> realInstancePtr(const Args& args) = 0;
};

template<typename cls, class... args>
struct TypeConstructor: public Constructor<cls> {
    TypeConstructor(): m_args_type_list_({
        TypeBase::getTypePtr<args>()...
    }) {
        m_constructor_wrapper_ = [](args&&... constructor_args) -> cls {
            return cls(std::forward<args>(constructor_args)...);
        };

        m_ptr_constructor_wrapper_ = [](args&&... constructor_args) -> std::shared_ptr<cls>{
            return std::make_shared<cls>(std::forward<args>(constructor_args)...);
        };
    }

    cls realInvoke(args&&... constructor_args) {
        return m_constructor_wrapper_(std::forward<args>(constructor_args)...);
    }

    std::shared_ptr<cls> realInvokePtr(args&&... constructor_args) {
        return m_ptr_constructor_wrapper_(std::forward<args>(constructor_args)...);
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

    std::any instanceObj(const Args& constructor_args) override {
        if (constructor_args.size() != m_args_type_list_.size()) {
            return {};
        }

        return createObjWithArgs(constructor_args, std::make_index_sequence<sizeof...(args)>());
    }

    std::shared_ptr<void> instanceObjPtr(const Args& constructor_args) override {
        return realInstancePtr(constructor_args);
    }

  protected:
    cls realInstance(const Args& constructor_args) override {
        return createObjWithArgs(constructor_args, std::make_index_sequence<sizeof...(args)>());
    }

    std::shared_ptr<cls> realInstancePtr(const Args& constructor_args) override {
        if (constructor_args.size() != m_args_type_list_.size()) {
            return {};
        }

        return createObjPtrWithArgs(constructor_args, std::make_index_sequence<sizeof...(args)>());
    }
  private:
    template<size_t... index>
    cls createObjWithArgs(const Args& constructor_args, std::index_sequence<index...> _) {
        return m_constructor_wrapper_(ArgUtils::convertArg<args>(constructor_args, index)...);
    }

    template<size_t... index>
    std::shared_ptr<cls> createObjPtrWithArgs(const Args& constructor_args, std::index_sequence<index...> _) {
        return m_ptr_constructor_wrapper_(ArgUtils::convertArg<args>(constructor_args, index)...);
    }

    std::function<cls(args...)> m_constructor_wrapper_;
    std::function<std::shared_ptr<cls>(args...)> m_ptr_constructor_wrapper_;
    std::vector<TypeBase*> m_args_type_list_;
};
}  // namespace Refl