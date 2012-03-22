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
