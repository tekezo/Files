#pragma once

#define IOLOG_DEBUG(...)                               \
  {                                                    \
    if (!org_pqrs_mykext::IOLogWrapper::suppressed()) {  \
      if (Config::get_debug()) {                       \
        IOLog("org.pqrs.mykext --Debug-- " __VA_ARGS__); \
      }                                                \
    }                                                  \
  }
#define IOLOG_DEBUG_POINTING(...)                      \
  {                                                    \
    if (!org_pqrs_mykext::IOLogWrapper::suppressed()) {  \
      if (Config::get_debug_pointing()) {              \
        IOLog("org.pqrs.mykext --Debug-- " __VA_ARGS__); \
      }                                                \
    }                                                  \
  }
#define IOLOG_DEVEL(...)                               \
  {                                                    \
    if (!org_pqrs_mykext::IOLogWrapper::suppressed()) {  \
      if (Config::get_debug_devel()) {                 \
        IOLog("org.pqrs.mykext --Devel-- " __VA_ARGS__); \
      }                                                \
    }                                                  \
  }

#define IOLOG_ERROR(...)                              \
  {                                                   \
    if (!org_pqrs_mykext::IOLogWrapper::suppressed()) { \
      IOLog("org.pqrs.mykext --Error-- " __VA_ARGS__);  \
    }                                                 \
  }

#define IOLOG_INFO(...)                               \
  {                                                   \
    if (!org_pqrs_mykext::IOLogWrapper::suppressed()) { \
      IOLog("org.pqrs.mykext --Info-- " __VA_ARGS__);   \
    }                                                 \
  }

#define IOLOG_WARN(...)                               \
  {                                                   \
    if (!org_pqrs_mykext::IOLogWrapper::suppressed()) { \
      IOLog("org.pqrs.mykext --Warn-- " __VA_ARGS__);   \
    }                                                 \
  }

// ------------------------------------------------------------
namespace org_pqrs_mykext {
class IOLogWrapper {
public:
  static bool suppressed(void) { return suppressed_; }
  static void suppress(bool v) { suppressed_ = v; }

  class ScopedSuppress {
  public:
    ScopedSuppress(void) {
      original = suppressed();
      suppress(true);
    }
    ~ScopedSuppress(void) { suppress(original); }

  private:
    bool original;
  };

private:
  static bool suppressed_;
};
}
