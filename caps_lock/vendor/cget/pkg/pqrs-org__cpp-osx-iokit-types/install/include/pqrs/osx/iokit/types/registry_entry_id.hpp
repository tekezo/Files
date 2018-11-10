#pragma once

// (C) Copyright Takayama Fumihiko 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <type_safe/strong_typedef.hpp>
#include <unordered_set>

namespace pqrs {
namespace osx {
namespace iokit {
struct registry_entry_id : type_safe::strong_typedef<registry_entry_id, uint64_t>,
                           type_safe::strong_typedef_op::equality_comparison<registry_entry_id>,
                           type_safe::strong_typedef_op::relational_comparison<registry_entry_id> {
  using strong_typedef::strong_typedef;
};

inline std::ostream& operator<<(std::ostream& stream, const registry_entry_id& value) {
  return stream << type_safe::get(value);
}
} // namespace iokit
} // namespace osx
} // namespace pqrs

namespace std {
template <>
struct hash<pqrs::osx::iokit::registry_entry_id> : type_safe::hashable<pqrs::osx::iokit::registry_entry_id> {
};
} // namespace std
