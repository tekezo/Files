#include <exception>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
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
    BOOST_FOREACH(const char* xmlfilepath, paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml(xmlfilepath, pt, false)) {
        continue;
      }

      traverse_appdef_(pt);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide appdef.xml is not loaded.)
      retval = true;
    }

    return true;
  }

  void
  xmlcompiler::traverse_appdef_(const boost::property_tree::ptree& pt)
  {
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
      if (it->first != "appdef") {
        traverse_appdef_(it->second);
      } else {
        std::tr1::shared_ptr<appdef> newappdef(new appdef);
        if (! newappdef) continue;

        boost::optional<std::string> name = (it->second).get_optional<std::string>("appname");
        if (! name) continue;
        newappdef->set_name(boost::trim_copy(*name));

        BOOST_FOREACH(boost::property_tree::ptree::value_type const & v, (it->second).get_child("equal")) {
          newappdef->add_rule_equal(boost::trim_copy(v.second.data()));
        }
        BOOST_FOREACH(boost::property_tree::ptree::value_type const & v, (it->second).get_child("prefix")) {
          newappdef->add_rule_prefix(boost::trim_copy(v.second.data()));
        }

        // Adding to keycode_ if name is not found.
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
    BOOST_FOREACH(std::string const & r, rules_equal_) {
      if (identifier == r) {
        return true;
      }
    }
    BOOST_FOREACH(std::string const & r, rules_prefix_) {
      if (identifier.compare(0, r.length(), r)) {
        return true;
      }
    }

    return false;
  }
}
