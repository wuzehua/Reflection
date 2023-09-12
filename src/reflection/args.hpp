#pragma once

#include <vector>
#include <any>

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
