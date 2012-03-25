#include <exception>
#include <boost/algorithm/string.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  bool
  xmlcompiler::reload_devicedef_(void)
  {
    bool retval = false;

    const char* paths[] = {
      "/Users/tekezo/Library/Application Support/KeyRemap4MacBook/private.xml",
      "/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/devicevendordef.xml",
      "/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/deviceproductdef.xml",
    };
    for (auto& xmlfilepath : paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml_(xmlfilepath, pt, true)) {
        continue;
      }

      traverse_devicedef_(pt);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide devicedef.xml is not loaded.)
      retval = true;
    }

    return retval;
  }

  void
  xmlcompiler::traverse_devicedef_(const boost::property_tree::ptree& pt)
  {
    for (auto& it : pt) {
      if (it.first != "devicevendordef" &&
          it.first != "deviceproductdef") {
        traverse_devicedef_(it.second);
      } else {
        std::string type;
        std::string name;
        boost::optional<uint32_t> value;

        if (it.first == "devicevendordef") {
          type = "DeviceVendor";
        } else if (it.first == "deviceproductdef") {
          type = "DeviceProduct";
        } else {
          throw xmlcompiler_logic_error("unknown type in traverse_devicedef_");
        }

        for (auto& child : it.second) {
          if (child.first == "vendorname" ||
              child.first == "productname") {
            name = boost::trim_copy(child.second.data());
          } else if (child.first == "vendorid" ||
                     child.first == "productid") {
            value = pqrs::string::to_uint32_t(boost::trim_copy(child.second.data()));
          }
        }

        if (name.empty() || ! value) {
          continue;
        }

        // Adding to symbolmap_ if name is not found.
        if (! symbolmap_.exists(type, name)) {
          symbolmap_.add(type, name, *value);
        }
      }
    }
  }
}
