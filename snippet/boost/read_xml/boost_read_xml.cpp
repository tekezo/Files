#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

void
traverse(const boost::property_tree::ptree& pt)
{
  for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
    std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
    traverse(it->second);
  }
}

void
read_xml_from_string(void)
{
  boost::property_tree::ptree pt;
  std::string xml = "<?xml version=\"1.0\"?>"
                    "<root>"
                    "<str id=\"str1\">Hello &#8594; World</str>"
                    "<str id=\"str2\">Hello2</str>"
                    "</root>";
  std::stringstream istream(xml, std::stringstream::in);

  boost::property_tree::read_xml(istream, pt);

  if (boost::optional<std::string> str = pt.get_optional<std::string>("root.str")) {
    std::cout << str.get() << std::endl;
  }

  BOOST_FOREACH(const boost::property_tree::ptree::value_type & child, pt.get_child("root")) {
    std::cout << child.first.data() << " " << child.second.data() << std::endl;
    if (boost::optional<std::string> id = child.second.get_optional<std::string>("<xmlattr>.id")) {
      std::cout << "id: " << id.get() << std::endl;
    }
  }
}

void
read_xml_from_file(void)
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml("/Library/org.pqrs/KeyRemap4MacBook/prefpane/checkbox.xml", pt);
  traverse(pt);
}

int
main(void)
{
  read_xml_from_string();
  read_xml_from_file();

  return 0;
}
