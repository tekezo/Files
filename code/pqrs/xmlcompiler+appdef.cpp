#include <exception>
#include <boost/algorithm/string.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  bool
  xmlcompiler::reload_appdef_(void)
  {
    bool retval = false;

    app_.clear();

    const char* paths[] = {
      "/Users/tekezo/Library/Application Support/KeyRemap4MacBook/private.xml",
      "/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/appdef.xml",
    };
    for (auto xmlfilepath : paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml_(xmlfilepath, pt, true)) {
        continue;
      }

      traverse_appdef_(pt);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide appdef.xml is not loaded.)
      retval = true;
    }

    return retval;
  }

  void
  xmlcompiler::traverse_appdef_(const boost::property_tree::ptree& pt)
  {
    for (auto it : pt) {
      if (it.first != "appdef") {
        traverse_appdef_(it.second);
      } else {
        std::tr1::shared_ptr<appdef> newappdef(new appdef);
        if (! newappdef) continue;

        for (auto child : it.second) {
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
        if (! symbolmap_.get("ApplicationType", newappdef->get_name())) {
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
    for (auto r : rules_equal_) {
      if (identifier == r) {
        return true;
      }
    }
    for (auto r : rules_prefix_) {
      if (identifier.compare(0, r.length(), r)) {
        return true;
      }
    }

    return false;
  }
}
