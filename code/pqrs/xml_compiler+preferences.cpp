#include "pqrs/xml_compiler.hpp"

namespace pqrs {
  void
  xml_compiler::reload_preferences_(void)
  {
    preferences_checbox_nodes_.clear();
    preferences_number_nodes_.clear();

    std::vector<xml_file_path_ptr> xml_file_path_ptrs;
    xml_file_path_ptrs.push_back(
      xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::private_xml, "private.xml")));
    xml_file_path_ptrs.push_back(
      xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::system_xml,  "checkbox.xml")));
  }

  void
  xml_compiler::traverse_preferences_:(NSXMLElement*)element stringForFilter:(NSString*)stringForFilter

}
