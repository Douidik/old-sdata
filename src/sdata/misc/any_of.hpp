#ifndef SDATA_ANY_OF_HPP
#define SDATA_ANY_OF_HPP

#include <concepts>

namespace sdata {

// Variadic version of std::same_as
template <typename T, typename... U>
concept any_of = (std::same_as<T, U> || ...);

}

#endif
