#include <exception>
#include <iostream>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "pqrs/string.hpp"
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  xmlcompiler::xmlcompiler(void) {}

  bool
  xmlcompiler::reload(void)
  {
    reload_replacementdef_();
    return true;
  }

  bool
  xmlcompiler::read_xml(const char* xmlfilepath, boost::property_tree::ptree& pt, bool with_replacement)
  {
    try {
      int flags = boost::property_tree::xml_parser::no_comments;

      if (! with_replacement) {
        boost::property_tree::read_xml(xmlfilepath, pt, flags, std::locale::classic());
      } else {
        std::string xml;
        pqrs::string::string_by_replacing_double_curly_braces_from_file(xml, xmlfilepath, replacement_);
        std::stringstream istream(xml, std::stringstream::in);
        boost::property_tree::read_xml(istream, pt, flags);
      }

      return true;

    } catch (std::exception& e) {
      errormessage_ = e.what();
    }
    return false;
  }
}
