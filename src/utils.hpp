#pragma once

#include <memory>
#include <variant>

#define VISIT_STMT(arg) auto operator()(arg const &stmt)->void
#define VISIT_EXPR(arg) auto operator()(arg const &expr)->std::any

template <typename Variant, typename Child, typename... Args>
auto constexpr inline make_unique_variant(Args &&...args) {
  return std::make_unique<Variant>(std::in_place_type<Child>,
                                   std::forward<Args>(args)...);
}
