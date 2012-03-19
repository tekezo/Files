#include <exception>
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
    for (auto xmlfilepath : paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml_(xmlfilepath, pt, false)) {
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
    for (auto it : pt) {
      if (it.first != "replacementdef") {
        traverse_replacementdef_(it.second);
      } else {
        std::string name;
        std::string value;
        for (auto child : it.second) {
          if (child.first == "replacementname") {
            name = child.second.data();
          } else if (child.first == "replacementvalue") {
            value = child.second.data();
          }
        }
        if (name.empty() || value.empty()) {
          continue;
        }
        // Adding to replacement_ if name is not found.
        if (replacement_.find(name) == replacement_.end()) {
          replacement_[name] = value;
        }
      }
    }
  }
}
