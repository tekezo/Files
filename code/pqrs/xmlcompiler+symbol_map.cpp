#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  xmlcompiler::symbol_map::symbol_map(void)
  {
    clear();
  }

  void
  xmlcompiler::symbol_map::clear(void)
  {
    symbol_map_.clear();
    symbol_map_["ConfigIndex::VK__AUTOINDEX__BEGIN__"] = 0;
  }

  uint32_t
  xmlcompiler::symbol_map::get(const std::string& name) const
  {
    auto it = symbol_map_.find(name);
    if (it == symbol_map_.end()) {
      throw xmlcompiler_runtime_error("Unknown symbol: " + name);
    }

    return it->second;
  }

  uint32_t
  xmlcompiler::symbol_map::get(const std::string& type, const std::string& name) const
  {
    return get(type + "::" + name);
  }

  bool
  xmlcompiler::symbol_map::exists(const std::string& name) const
  {
    return symbol_map_.find(name) != symbol_map_.end();
  }

  bool
  xmlcompiler::symbol_map::exists(const std::string& type, const std::string& name) const
  {
    return exists(type + "::" + name);
  }

  void
  xmlcompiler::symbol_map::add(const std::string& type, const std::string& name, uint32_t value)
  {
    auto n = type + "::" + name;

    auto it = symbol_map_.find(n);
    if (it != symbol_map_.end()) {
      xmlcompiler_logic_error("Symbol is already registered: " + n);
    }

    symbol_map_[n] = value;
  }

  void
  xmlcompiler::symbol_map::add(const std::string& type, const std::string& name)
  {
    auto n = type + "::VK__AUTOINDEX__BEGIN__";
    auto v = get(n);
    symbol_map_[n] = v + 1;
    return add(type, name, v);
  }

  // ============================================================
  void
  xmlcompiler::reload_symbol_map_(void)
  {
    symbol_map_.clear();

    std::vector<xml_file_path_ptr> xml_file_path_ptrs;
    xml_file_path_ptrs.push_back(
      xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::system_xml,  "symbol_map.xml")));

    std::vector<ptree_ptr> pt_ptrs;
    read_xmls_(pt_ptrs, xml_file_path_ptrs);

    for (auto pt_ptr : pt_ptrs) {
      traverse_symbol_map_(*pt_ptr);
    }
  }

  void
  xmlcompiler::traverse_symbol_map_(const boost::property_tree::ptree& pt)
  {
    for (auto& it : pt) {
      if (it.first != "symbol_map") {
        traverse_symbol_map_(it.second);

      } else {
        try {
          auto value = pqrs::string::to_uint32_t(it.second.get<std::string>("<xmlattr>.value"));
          if (! value) {
            set_error_message_("Invalid value: " + it.second.data());
            continue;
          }

          symbol_map_.add(it.second.get<std::string>("<xmlattr>.type"),
                          it.second.get<std::string>("<xmlattr>.name"),
                          *value);
        } catch (std::exception& e) {
          set_error_message_(e.what());
        }
      }
    }
  }
}
