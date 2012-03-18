#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

void
read_xml_from_string(void)
{
  boost::property_tree::ptree pt;
  std::string xml = "<?xml version=\"1.0\"?><root><str>Hello &#8594; World</str></root>";
  std::stringstream istream(xml, std::stringstream::in);

  boost::property_tree::read_xml(istream, pt);

  if (boost::optional<std::string> str = pt.get_optional<std::string>("root.str")) {
    std::cout << str.get() << std::endl;
  }
}

int
main(void)
{
  read_xml_from_string();

  return 0;
}
