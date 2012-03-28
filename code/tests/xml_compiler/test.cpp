#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include "pqrs/xml_compiler_bindings_clang.h"
#include "pqrs/xml_compiler.hpp"
#include "pqrs/bridge.h"

TEST(pqrs_xml_compiler, reload)
{
  pqrs::xml_compiler xml_compiler("data/system_xml", "data/private_xml");
  xml_compiler.reload();
  EXPECT_EQ(boost::optional<uint32_t>(123), xml_compiler.get_symbol_map_value("KeyCode::MY_LANG_KEY"));
  EXPECT_EQ(boost::optional<uint32_t>(2), xml_compiler.get_symbol_map_value("ConsumerKeyCode::BRIGHTNESS_UP"));
}

TEST(pqrs_xml_compiler, reload_bindings_clang)
{
  struct pqrs_xml_compiler* p = NULL;
  EXPECT_EQ(0, pqrs_xml_compiler_initialize(&p, "data/system_xml", "data/private_xml"));
  pqrs_xml_compiler_reload(p);
  pqrs_xml_compiler_terminate(&p);
}

TEST(pqrs_xml_compiler, reload_invalid_xml)
{
  {
    struct pqrs_xml_compiler* p = NULL;
    EXPECT_EQ(0, pqrs_xml_compiler_initialize(&p, "data/system_xml", "data/invalid_xml/private_xml"));
    pqrs_xml_compiler_reload(p);
    EXPECT_EQ("<private.xml>(4): expected element name", std::string(pqrs_xml_compiler_get_error_message(p)));
    pqrs_xml_compiler_terminate(&p);
  }

  {
    struct pqrs_xml_compiler* p = NULL;
    EXPECT_EQ(0, pqrs_xml_compiler_initialize(&p, "data/invalid_xml/symbol_map_xml_no_type", "data/private_xml"));
    pqrs_xml_compiler_reload(p);
    EXPECT_EQ("No 'type' Attribute found within <symbol_map>.", std::string(pqrs_xml_compiler_get_error_message(p)));
    EXPECT_EQ(1, pqrs_xml_compiler_get_error_count(p));
    pqrs_xml_compiler_terminate(&p);
  }
  {
    struct pqrs_xml_compiler* p = NULL;
    EXPECT_EQ(0, pqrs_xml_compiler_initialize(&p, "data/invalid_xml/symbol_map_xml_no_name", "data/private_xml"));
    pqrs_xml_compiler_reload(p);
    EXPECT_EQ("No 'name' Attribute found within <symbol_map>.", std::string(pqrs_xml_compiler_get_error_message(p)));
    EXPECT_EQ(1, pqrs_xml_compiler_get_error_count(p));
    pqrs_xml_compiler_terminate(&p);
  }
  {
    struct pqrs_xml_compiler* p = NULL;
    EXPECT_EQ(0, pqrs_xml_compiler_initialize(&p, "data/invalid_xml/symbol_map_xml_no_value", "data/private_xml"));
    pqrs_xml_compiler_reload(p);
    EXPECT_EQ("No 'value' Attribute found within <symbol_map>.", std::string(pqrs_xml_compiler_get_error_message(p)));
    EXPECT_EQ(1, pqrs_xml_compiler_get_error_count(p));
    pqrs_xml_compiler_terminate(&p);
  }
}

TEST(pqrs_xml_compiler_symbol_map, add)
{
  pqrs::xml_compiler::symbol_map s;
  s.add("KeyCode", "SPACE", 36);
  s.add("KeyCode", "VK__AUTOINDEX__BEGIN__", 1024);
  s.add("KeyCode", "VK_NEW1");
  s.add("KeyCode", "VK_NEW2");

  EXPECT_EQ(static_cast<uint32_t>(1024), s.get("KeyCode::VK_NEW1"));
  EXPECT_EQ(static_cast<uint32_t>(1025), s.get("KeyCode::VK_NEW2"));
}

TEST(pqrs_xml_compiler_remapclasses_initialize_vector, add)
{
  pqrs::xml_compiler::remapclasses_initialize_vector v;
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

TEST(pqrs_xml_compiler_filter_vector, filter_vector)
{
  pqrs::xml_compiler::symbol_map s;
  s.add("ApplicationType", "APP1", 1);
  s.add("ApplicationType", "APP2", 2);
  s.add("ApplicationType", "APP3", 3);
  s.add("DeviceVendor", "VENDOR1", 10);
  s.add("DeviceVendor", "VENDOR2", 20);
  s.add("DeviceVendor", "VENDOR3", 30);
  s.add("DeviceProduct", "PRODUCT1", 100);
  s.add("DeviceProduct", "PRODUCT2", 200);
  s.add("DeviceProduct", "PRODUCT3", 300);
  s.add("ConfigIndex", "config1", 1000);
  s.add("ConfigIndex", "config2", 2000);
  s.add("ConfigIndex", "config3", 3000);
  s.add("ModifierFlag", "MOD1", 0x1000);
  s.add("ModifierFlag", "MOD2", 0x2000);
  s.add("ModifierFlag", "MOD3", 0x4000);

  std::string xml("<?xml version=\"1.0\"?>"
                  "<item>"
                  "  <only>APP1,APP3</only>"
                  "  <not><!-- XXX --->APP2</not>"
                  "  <identifier>sample</identifier>"
                  "  <device_only>DeviceVendor::VENDOR1, DeviceProduct::PRODUCT1, </device_only>"
                  "  <device_not>"
                  "    DeviceVendor::VENDOR3,,,,"
                  "    DeviceProduct::PRODUCT3,"
                  "  </device_not>"
                  "  <config_only>config1,config2</config_only>"
                  "  <config_not>config3</config_not>"
                  "  <modifier_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifier_only>"
                  "  <modifier_not> ModifierFlag::MOD2 </modifier_not>"
                  "</item>");
  std::stringstream istream(xml, std::stringstream::in);

  int flags = boost::property_tree::xml_parser::no_comments;
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml(istream, pt, flags);

  for (auto& it : pt) {
    pqrs::xml_compiler::filter_vector fv(s, it.second);

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

    // <config_only>config1,config2</config_only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_CONFIG_ONLY);
    expected.push_back(1000);
    expected.push_back(2000);

    // <config_not>config3</config_not>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_CONFIG_NOT);
    expected.push_back(3000);

    // <modifier_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifier_only>
    expected.push_back(2);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_ONLY);
    expected.push_back(0x1000 | 0x4000);

    // <modifier_not> ModifierFlag::MOD2 </modifier_not>
    expected.push_back(2);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_NOT);
    expected.push_back(0x2000);

    EXPECT_EQ(expected, fv.get());
  }
}
