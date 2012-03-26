#include <exception>
#include <boost/algorithm/string.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  void
  xmlcompiler::reload_devicedef_(void)
  {
    std::vector<std::string> xmlfilepaths;
    xmlfilepaths.push_back(private_xml_directory_ + "/private.xml");
    xmlfilepaths.push_back(system_xml_directory_ + "/devicevendordef.xml");
    xmlfilepaths.push_back(system_xml_directory_ + "/deviceproductdef.xml");

    std::vector<ptree_ptr> pt_ptrs;
    read_xmls_(pt_ptrs, xmlfilepaths);

    for (auto pt_ptr : pt_ptrs) {
      traverse_devicedef_(*pt_ptr);
    }
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
