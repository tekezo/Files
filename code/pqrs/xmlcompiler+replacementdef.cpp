#include <exception>
#include <boost/foreach.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  bool
  xmlcompiler::reload_replacementdef_(void)
  {
    bool retval = false;

    replacement_.clear();

    const char* paths[] = {
      "/Users/tekezo/Library/Application Support/KeyRemap4MacBook/private.xml",
      "/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/replacementdef.xml",
    };
    BOOST_FOREACH(const char* xmlfilepath, paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml(xmlfilepath, pt, false)) {
        continue;
      }

      traverse_replacementdef_(pt);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide replacementdef.xml is not loaded.)
      retval = true;
    }

    return true;
  }

  void
  xmlcompiler::traverse_replacementdef_(const boost::property_tree::ptree& pt)
  {
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
      if (it->first != "replacementdef") {
        traverse_replacementdef_(it->second);
      } else {
        boost::optional<std::string> name  = (it->second).get_optional<std::string>("replacementname");
        if (! name) continue;
        boost::optional<std::string> value = (it->second).get_optional<std::string>("replacementvalue");
        if (! value) continue;

        // Adding to replacement_ if name is not found.
        if (replacement_.find(*name) == replacement_.end()) {
          replacement_[*name] = *value;
        }
      }
    }
  }
}
