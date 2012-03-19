#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  void
  xmlcompiler::symbolmap_keycode::clear(void)
  {
    symbolmap_.clear();
  }

  boost::optional<unsigned int>
  xmlcompiler::symbolmap_keycode::get(const std::string& name)
  {
    auto it = symbolmap_.find(name);
    if (it == symbolmap_.end()) {
      return boost::none;
    }

    return it->second;
  }

  boost::optional<unsigned int>
  xmlcompiler::symbolmap_keycode::get(const std::string& type, const std::string& name)
  {
    return get(type + "::" + name);
  }

  bool
  xmlcompiler::symbolmap_keycode::append(const std::string& type, const std::string& name, unsigned int value)
  {
    auto n = type + "::" + name;
    if (get(n)) {
      return false;
    }

    symbolmap_[n] = value;
    std::cout << "append: " << n << " " << value << std::endl;
    return true;
  }

  bool
  xmlcompiler::symbolmap_keycode::append(const std::string& type, const std::string& name)
  {
    auto n = type + "::VK__AUTOINDEX__BEGIN__";
    auto v = get(n);
    if (! v) {
      return false;
    }
    symbolmap_[n] = *v + 1;
    return append(type, name, *v);
  }
}
