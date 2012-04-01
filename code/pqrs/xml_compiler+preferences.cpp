#include "pqrs/xml_compiler.hpp"

namespace pqrs {
  xml_compiler::preferences_checkbox_node::preferences_checkbox_node(void) :
    name_line_count_(0)
  {}

  bool
  xml_compiler::preferences_checkbox_node::handle_name_and_appendix(boost::property_tree::ptree::const_iterator& it,
                                                                    std::string& name,
                                                                    int& name_line_count)
  {
    if (it->first == "name") {
      name += (it->second).data();
      name += "\n";

      ++name_line_count;

      return true;

    } else if (it->first == "appendix") {
      name += "  ";
      name += (it->second).data();
      name += "\n";

      ++name_line_count;

      return true;
    }

    return false;
  }

  xml_compiler::preferences_number_node::preferences_number_node(void) :
    name_line_count_(0),
    default_value_(0),
    step_(1)
  {}

  void
  xml_compiler::reload_preferences_(void)
  {
    preferences_checkbox_nodes_.clear();
    preferences_number_nodes_.clear();

    // checkbox
    {
      std::vector<xml_file_path_ptr> xml_file_path_ptrs;
      xml_file_path_ptrs.push_back(
        xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::private_xml, "private.xml")));
      xml_file_path_ptrs.push_back(
        xml_file_path_ptr(new xml_file_path(xml_file_path::base_directory::system_xml,  "checkbox.xml")));

      std::vector<ptree_ptr> pt_ptrs;
      read_xmls_(pt_ptrs, xml_file_path_ptrs);

      for (auto& pt_ptr : pt_ptrs) {
        traverse_preferences_(*pt_ptr, preferences_checkbox_nodes_);
      }
    }
  }

  void
  xml_compiler::traverse_preferences_(const boost::property_tree::ptree& pt,
                                      std::vector<preferences_checkbox_node_ptr>& preferences_nodes)
  {
    for (auto& it : pt) {
      if (it.first != "item") {
        traverse_preferences_(it.second, preferences_nodes);
      }
    }
  }
}
