#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <utility>

#include "type.hpp"
#include "type_union.hpp"

namespace Refl {
template<typename, bool>
struct Method;

namespace MethodUtils {
template<typename cls, bool is_static, typename ret_type, class... args>
std::optional<ret_type> invokeMethod(Method<cls, is_static>* method, cls* obj, args&&... func_args);

template<typename cls, bool is_static, class... args>
void invokeVoidMethod(Method<cls, is_static>* method, cls* obj, args&&... func_args);
}  // namespace MethodUtils

template<typename cls, bool is_static>
struct Method {
    [[nodiscard]] std::string getName() const { return m_method_name_; }
    [[nodiscard]] bool isStaticMethod() const { return m_is_static_method_; }
    [[nodiscard]] bool isConstMethod() const { return m_is_const_method_; }

    template<typename ret_type, class... args>
    typename std::enable_if<!std::is_same_v<ret_type, void>, std::optional<ret_type>>::type invoke(cls* obj, args&&... func_args) {
        return MethodUtils::invokeMethod<cls, is_static, ret_type, args...>(this, obj, std::forward<args>(func_args)...);
    }

    template<typename ret_type, class... args>
    typename std::enable_if<std::is_same_v<ret_type, void>, void>::type invoke(cls* obj, args&&... func_args) {
        MethodUtils::invokeVoidMethod<cls, is_static, args...>(this, obj, std::forward<args>(func_args)...);
    }

    template<typename ret_type, class... args>
    typename std::enable_if<!std::is_same_v<ret_type, void>, std::optional<ret_type>>::type invokeWithRetType(
                                                                                            cls* obj,
                                                                                            const Type<ret_type>&,
                                                                                            args&&... func_args) {
        return MethodUtils::invokeMethod<cls, is_static, ret_type, args...>(this, obj, std::forward<args>(func_args)...);
    }

    template<typename ret_type, class... args>
    typename std::enable_if<std::is_same_v<ret_type, void>, void>::type invokeWithRetType(
                                                                                        cls* obj,
                                                                                        const Type<ret_type>&,
                                                                                        args&&... func_args) {
        MethodUtils::invokeVoidMethod<cls, is_static, args...>(this, obj, std::forward<args>(func_args)...);
    }
  protected:
    std::string m_method_name_;
    bool m_is_static_method_{false};
    bool m_is_const_method_{false};
};

template<typename cls, bool is_static, typename ret_type, class... args>
struct TypeMethod: public Method<cls, is_static> {
    using MethodPtr = ret_type (cls::*)(args...);
    using MethodConstPtr = ret_type (cls::*)(args...) const;
    using MethodStaticPtr = ret_type (*)(args...);

    TypeMethod(const std::string& name, MethodPtr p) {
        this->m_method_name_ = name;
        this->m_method_ptr_.first.first = p;
        this->m_is_static_method_ = is_static;
        this->m_is_const_method_ = false;
    }

    TypeMethod(const std::string& name, MethodConstPtr p) {
        this->m_method_name_ = name;
        this->m_method_ptr_.first.second = p;
        this->m_is_static_method_ = is_static;
        this->m_is_const_method_ = false;
    }

    TypeMethod(const std::string& name, MethodStaticPtr p) {
        this->m_method_name_ = name;
        this->m_method_ptr_.second = p;
        this->m_is_static_method_ = is_static;
        this->m_is_const_method_ = false;
    }

    template<bool is_static_ = is_static, typename R = ret_type>
    typename std::enable_if<is_static_ && std::is_same_v<R, void>, void>::type realInvoke(cls* obj, args&&... func_args) {
        (*(m_method_ptr_.second))(std::forward<args>(func_args)...);
    }

    template<bool is_static_ = is_static, typename R = ret_type>
    typename std::enable_if<!is_static_ && std::is_same_v<R, void>, void>::type realInvoke(cls* obj, args&&... func_args) {
        if (obj == nullptr) {
            return;
        }

        if (this->m_is_const_method_) {
            (obj->*(m_method_ptr_.first.second))(std::forward<args>(func_args)...);
        } else {
            (obj->*(m_method_ptr_.first.first))(std::forward<args>(func_args)...);
        }
    }

    template<bool is_static_ = is_static, typename R = ret_type>
    typename std::enable_if<is_static_ && !std::is_same_v<R, void>, std::optional<R>>::type realInvoke(cls* obj, args&&... func_args) {
        return std::make_optional<R>((*(m_method_ptr_.second))(std::forward<args>(func_args)...));
    }

    template<bool is_static_ = is_static, typename R = ret_type>
    typename std::enable_if<!is_static_ && !std::is_same_v<R, void>, std::optional<R>>::type realInvoke(cls* obj, args&&... func_args) {
        if (obj == nullptr) {
            return std::nullopt;
        }

        if (this->m_is_const_method_) {
            return std::make_optional<R>((obj->*(m_method_ptr_.first.second))(std::forward<args>(func_args)...));
        } else {
            return std::make_optional<R>((obj->*(m_method_ptr_.first.first))(std::forward<args>(func_args)...));
        }
    }

  private:
    TypeUnion<TypeUnion<MethodPtr, MethodConstPtr>, MethodStaticPtr> m_method_ptr_;
};

namespace MethodUtils {
template<typename cls, bool is_static, typename ret_type, class... args>
std::optional<ret_type> invokeMethod(Method<cls, is_static>* method, cls* obj, args&&... func_args) {
    auto real_method = static_cast<TypeMethod<cls, is_static, ret_type, args...>*>(method);
    return real_method->realInvoke(obj, std::forward<args>(func_args)...);
}

template<typename cls, bool is_static, class... args>
void invokeVoidMethod(Method<cls, is_static>* method, cls* obj, args&&... func_args) {
    auto real_method = static_cast<TypeMethod<cls, is_static, void, args...>*>(method);
    real_method->realInvoke(obj, std::forward<args>(func_args)...);
}
}  // namespace MethodUtils
}  // namespace Refl