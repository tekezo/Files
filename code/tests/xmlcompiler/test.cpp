#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include "pqrs/xmlcompiler.hpp"
#include "pqrs/bridge.h"

TEST(pqrs_xmlcompiler, reload)
{
  pqrs::xmlcompiler xmlcompiler;
  xmlcompiler.reload();
}

TEST(pqrs_xmlcompiler_symbolmap, add)
{
  pqrs::xmlcompiler::symbolmap s;
  EXPECT_TRUE(s.add("KeyCode", "SPACE", 36));
  EXPECT_TRUE(s.add("KeyCode", "VK__AUTOINDEX__BEGIN__", 1024));
  EXPECT_TRUE(s.add("KeyCode", "VK_NEW1"));
  EXPECT_TRUE(s.add("KeyCode", "VK_NEW2"));

  {
    boost::optional<uint32_t> expected = 1024;
    boost::optional<uint32_t> actual = s.get("KeyCode::VK_NEW1");
    EXPECT_EQ(expected, actual);
  }

  {
    boost::optional<uint32_t> expected = 1025;
    boost::optional<uint32_t> actual = s.get("KeyCode::VK_NEW2");
    EXPECT_EQ(expected, actual);
  }
}

TEST(pqrs_xmlcompiler_remapclasses_initialize_vector, add)
{
  pqrs::xmlcompiler::remapclasses_initialize_vector v;
  std::vector<uint32_t> v1;
  v.add(v1, 1);

  v1.push_back(1);
  v1.push_back(2);
  v1.push_back(3);
  v.add(v1, 3);

  v.freeze();

  std::vector<uint32_t> expected;
  expected.push_back(BRIDGE_REMAPCLASS_INITIALIZE_VECTOR_FORMAT_VERSION);
  expected.push_back(4); // count is {0,1,2,3}

  expected.push_back(1); // configindex:1
  expected.push_back(1);

  expected.push_back(4); // configindex:3
  expected.push_back(3);
  expected.push_back(1);
  expected.push_back(2);
  expected.push_back(3);

  expected.push_back(1); // configindex:0
  expected.push_back(0);

  expected.push_back(1); // configindex:2
  expected.push_back(2);

  EXPECT_EQ(expected, v.get());
}

TEST(pqrs_xmlcompiler_filter_vector, filter_vector)
{
  pqrs::xmlcompiler::symbolmap s;
  s.add("ApplicationType", "APP1", 1);
  s.add("ApplicationType", "APP2", 2);
  s.add("ApplicationType", "APP3", 3);
  s.add("DeviceVendor", "VENDOR1", 10);
  s.add("DeviceVendor", "VENDOR2", 20);
  s.add("DeviceVendor", "VENDOR3", 30);
  s.add("DeviceProduct", "PRODUCT1", 100);
  s.add("DeviceProduct", "PRODUCT2", 200);
  s.add("DeviceProduct", "PRODUCT3", 300);

  std::string xml("<?xml version=\"1.0\"?>"
                  "<item>"
                  "  <only>APP1,APP3</only>"
                  "  <not>APP2</not>"
                  "  <identifier>sample</identifier>"
                  "  <device_only>DeviceVendor::VENDOR1, DeviceProduct::PRODUCT1, </device_only>"
                  "  <device_not>"
                  "    DeviceVendor::VENDOR3,,,,"
                  "    DeviceProduct::PRODUCT3,"
                  "  </device_not>"
                  "</item>");
  std::stringstream istream(xml, std::stringstream::in);

  int flags = boost::property_tree::xml_parser::no_comments;
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml(istream, pt, flags);

  for (auto it : pt) {
    pqrs::xmlcompiler::filter_vector fv(s, it.second);

    std::vector<uint32_t> expected;

    // <only>APP1,APP3</only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_APPLICATION_ONLY);
    expected.push_back(1); // APP1
    expected.push_back(3); // APP3

    // <not>APP2</not>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_APPLICATION_NOT);
    expected.push_back(2); // APP2

    // <device_only>DeviceVendor::VENDOR1, DeviceProduct::PRODUCT1, </device_only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_DEVICE_ONLY);
    expected.push_back(10);
    expected.push_back(100);

    // <device_not>DeviceVendor::VENDOR3, DeviceProduct::PRODUCT3, </device_not>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_DEVICE_NOT);
    expected.push_back(30);
    expected.push_back(300);

    EXPECT_EQ(expected, fv.get());
  }
}
