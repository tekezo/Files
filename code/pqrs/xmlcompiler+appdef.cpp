#include <exception>
#include <boost/algorithm/string.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  void
  xmlcompiler::reload_appdef_(void)
  {
    app_.clear();

    std::vector<xml_file_path_ptr> xml_file_path_ptrs;
    xml_file_path_ptrs.push_back(
      xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::private_xml, "private.xml")));
    xml_file_path_ptrs.push_back(
      xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::system_xml,  "appdef.xml")));

    std::vector<ptree_ptr> pt_ptrs;
    read_xmls_(pt_ptrs, xml_file_path_ptrs);

    for (auto pt_ptr : pt_ptrs) {
      traverse_appdef_(*pt_ptr);
    }
  }

  void
  xmlcompiler::traverse_appdef_(const boost::property_tree::ptree& pt)
  {
    for (auto& it : pt) {
      if (it.first != "appdef") {
        traverse_appdef_(it.second);
      } else {
        std::tr1::shared_ptr<appdef> newappdef(new appdef);
        if (! newappdef) continue;

        for (auto& child : it.second) {
          if (child.first == "appname") {
            newappdef->set_name(boost::trim_copy(child.second.data()));
          } else if (child.first == "equal") {
            newappdef->add_rule_equal(boost::trim_copy(child.second.data()));
          } else if (child.first == "prefix") {
            newappdef->add_rule_prefix(boost::trim_copy(child.second.data()));
          }
        }

        if ((newappdef->get_name()).empty()) {
          continue;
        }

        // Adding to symbolmap_ if name is not found.
        if (! symbolmap_.exists("ApplicationType", newappdef->get_name())) {
          symbolmap_.add("ApplicationType", newappdef->get_name());
        }
      }
    }
  }

  void
  xmlcompiler::appdef::add_rule_equal(const std::string& v)
  {
    if (! v.empty()) {
      rules_equal_.push_back(v);
    }
  }

  void
  xmlcompiler::appdef::add_rule_prefix(const std::string& v)
  {
    if (! v.empty()) {
      rules_prefix_.push_back(v);
    }
  }

  bool
  xmlcompiler::appdef::is_rules_matched(const std::string& identifier)
  {
    for (auto& r : rules_equal_) {
      if (identifier == r) {
        return true;
      }
    }
    for (auto& r : rules_prefix_) {
      if (identifier.compare(0, r.length(), r)) {
        return true;
      }
    }

    return false;
  }
}
