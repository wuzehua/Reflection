#pragma once

#include <vector>
#include <any>

namespace Refl {
struct Args {
    Args(std::initializer_list<std::any> args): m_values_(args) {}

    template<class... arg_type>
    explicit Args(arg_type&&... args): Args(std::initializer_list<std::any>({ std::forward<arg_type>(args)... })) {}

    const std::any& operator[](size_t index) const {
        return m_values_[index];
    }

    [[nodiscard]] size_t size() const {
        return m_values_.size();
    }
  private:
    std::vector<std::any> m_values_;
};

namespace ArgUtils {
template<class... arg_type>
Args makeArgs(arg_type&&... args) {
    return std::move(Args(std::forward<arg_type>(args)...));
}

template<>
Args makeArgs() {
    return std::move(Args(std::initializer_list<std::any>()));
}

template<class T>
T convertArg(const Args& constructor_args, size_t index) {
    return std::any_cast<T>(constructor_args[index]);
}
}  // namespace ArgUtils
}  // namespace Refl
