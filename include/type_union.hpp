#pragma once

namespace Refl {
template<typename T, typename U>
union TypeUnion {
    T first;
    U second;
};
}  // namespace Refl