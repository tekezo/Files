#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  xmlcompiler::symbolmap::symbolmap(void)
  {
    clear();
  }

  void
  xmlcompiler::symbolmap::clear(void)
  {
    symbolmap_.clear();
    symbolmap_["ConfigIndex::VK__AUTOINDEX__BEGIN__"] = 0;
  }

  uint32_t
  xmlcompiler::symbolmap::get(const std::string& name) const
  {
    auto it = symbolmap_.find(name);
    if (it == symbolmap_.end()) {
      throw xmlcompiler_runtime_error("Unknown symbol: " + name);
    }

    return it->second;
  }

  uint32_t
  xmlcompiler::symbolmap::get(const std::string& type, const std::string& name) const
  {
    return get(type + "::" + name);
  }

  bool
  xmlcompiler::symbolmap::exists(const std::string& name) const
  {
    return symbolmap_.find(name) != symbolmap_.end();
  }

  bool
  xmlcompiler::symbolmap::exists(const std::string& type, const std::string& name) const
  {
    return exists(type + "::" + name);
  }

  void
  xmlcompiler::symbolmap::add(const std::string& type, const std::string& name, uint32_t value)
  {
    auto n = type + "::" + name;

    auto it = symbolmap_.find(n);
    if (it != symbolmap_.end()) {
      xmlcompiler_logic_error("Symbol is already registered: " + n);
    }

    symbolmap_[n] = value;
  }

  void
  xmlcompiler::symbolmap::add(const std::string& type, const std::string& name)
  {
    auto n = type + "::VK__AUTOINDEX__BEGIN__";
    auto v = get(n);
    symbolmap_[n] = v + 1;
    return add(type, name, v);
  }

  // ============================================================
  void
  xmlcompiler::reload_symbolmap_(void)
  {
    symbolmap_.clear();

    std::vector<std::string> xmlfilepaths;
    xmlfilepaths.push_back("/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/symbolmap.xml");

    std::vector<ptree_ptr> pt_ptrs;
    read_xmls_(pt_ptrs, xmlfilepaths);

    for (auto pt_ptr : pt_ptrs) {
      traverse_symbolmap_(*pt_ptr);
    }
  }

  void
  xmlcompiler::traverse_symbolmap_(const boost::property_tree::ptree& pt)
  {
    for (auto& it : pt) {
      if (it.first != "item") {
        traverse_symbolmap_(it.second);
      } else {
        auto value = pqrs::string::to_uint32_t(it.second.get<std::string>("<xmlattr>.value"));
        if (! value) {
          throw xmlcompiler_runtime_error("Invalid value in symbolmap.xml.");
        }
        symbolmap_.add(it.second.get<std::string>("<xmlattr>.type"),
                       it.second.get<std::string>("<xmlattr>.name"),
                       *value);
      }
    }
  }
}
