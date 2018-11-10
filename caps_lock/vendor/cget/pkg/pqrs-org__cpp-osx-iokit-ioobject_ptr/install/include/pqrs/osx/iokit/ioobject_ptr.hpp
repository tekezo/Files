#pragma once

// pqrs::ioobject_ptr v1.3.0

// (C) Copyright Takayama Fumihiko 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <IOKit/IOKitLib.h>

namespace pqrs {
namespace osx {
namespace iokit {
class ioobject_ptr final {
public:
  ioobject_ptr(void) : ioobject_ptr(IO_OBJECT_NULL) {
  }

  ioobject_ptr(io_object_t p) : p_(p) {
    if (p_) {
      IOObjectRetain(p_);
    }
  }

  ioobject_ptr(const ioobject_ptr& other) : p_(IO_OBJECT_NULL) {
    *this = other;
  }

  ioobject_ptr& operator=(const ioobject_ptr& other) {
    auto old = p_;

    p_ = other.p_;
    if (p_) {
      IOObjectRetain(p_);
    }

    if (old) {
      IOObjectRelease(old);
    }

    return *this;
  }

  ~ioobject_ptr(void) {
    reset();
  }

  const io_object_t& get(void) const {
    return p_;
  }

  io_object_t& get(void) {
    return const_cast<io_object_t&>((static_cast<const ioobject_ptr&>(*this)).get());
  }

  void reset(void) {
    if (p_) {
      IOObjectRelease(p_);
      p_ = IO_OBJECT_NULL;
    }
  }

  operator bool(void) const {
    return p_ != IO_OBJECT_NULL;
  }

  const io_object_t& operator*(void)const {
    return p_;
  }

  io_object_t& operator*(void) {
    return const_cast<io_object_t&>(*(static_cast<const ioobject_ptr&>(*this)));
  }

private:
  io_object_t p_;
};
} // namespace iokit
} // namespace osx
} // namespace pqrs
