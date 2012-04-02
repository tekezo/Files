#include "pqrs/xml_compiler.hpp"

namespace pqrs {
  xml_compiler::preferences_checkbox_node::preferences_checkbox_node(void) :
    name_line_count_(0)
  {}

  bool
  xml_compiler::preferences_checkbox_node::handle_item_child(const boost::property_tree::ptree::value_type& it)
  {
    return handle_name_and_appendix(it, name_, name_line_count_);
  }

  bool
  xml_compiler::preferences_checkbox_node::handle_name_and_appendix(const boost::property_tree::ptree::value_type& it,
                                                                    std::string& name,
                                                                    int& name_line_count)
  {
    if (it.first == "name") {
      name += (it.second).data();
      name += "\n";

      ++name_line_count;

      return true;

    } else if (it.first == "appendix") {
      name += "  ";
      name += (it.second).data();
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

  bool
  xml_compiler::preferences_number_node::handle_item_child(const boost::property_tree::ptree::value_type& it)
  {
    return preferences_checkbox_node::handle_name_and_appendix(it, name_, name_line_count_);
  }

  template <class T>
  void
  xml_compiler::preferences_node_tree<T>::clear(void)
  {
    if (children_) {
      children_->clear();
    }
  }

  template <class T>
  void
  xml_compiler::preferences_node_tree<T>::traverse_item(const boost::property_tree::ptree& pt)
  {
    for (auto& it : pt) {
      if (it.first != "item") {
        traverse_item(it.second);

      } else {
        preferences_node_tree_ptr ptr(new preferences_node_tree);

#if 0
        for (auto& child : it.second) {
          if ((ptr->node_).handle_item_child(child)) {
            continue;
          }

          if (child.first == "item") {
            ptr->traverse_item(child.second);
          }
        }

        if (! children_) {
          children_ = std::tr1::shared_ptr<std::vector<std::tr1::shared_ptr<preferences_node_tree<T > > > >(new std::vector<std::tr1::shared_ptr<T> >());
        }
        children_->push_back(ptr);
#endif
      }
    }
  }

  void
  xml_compiler::reload_preferences_(void)
  {
    preferences_checkbox_node_tree_.clear();
    preferences_number_node_tree_.clear();

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
        preferences_checkbox_node_tree_.traverse_item(*pt_ptr);
      }
    }
  }
}
