#include <cerrno>
#include <cstdlib>
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

  // ============================================================
  bool
  xmlcompiler::reload_symbolmap_(void)
  {
    symbolmap_keycode_.clear();

    const char* xmlfilepath = "/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/symbolmap.xml";

    boost::property_tree::ptree pt;
    if (! pqrs::xmlcompiler::read_xml_(xmlfilepath, pt, true)) {
      return false;
    }

    return traverse_symbolmap_(pt);
  }

  bool
  xmlcompiler::traverse_symbolmap_(const boost::property_tree::ptree& pt)
  {
    for (auto it : pt) {
      if (it.first != "item") {
        traverse_symbolmap_(it.second);
      } else {
        std::string value = it.second.get<std::string>("<xmlattr>.value");
        errno = 0;
        unsigned int v = strtol(value.c_str(), NULL, 0);
        if (errno != 0) {
          return false;
        }
        if (! symbolmap_keycode_.append(it.second.get<std::string>("<xmlattr>.type"),
                                        it.second.get<std::string>("<xmlattr>.name"),
                                        v)) {
          return false;
        }
      }
    }
    return true;
  }
}
