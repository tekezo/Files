#include <gtest/gtest.h>
#include "pqrs/xmlcompiler.hpp"

TEST(pqrs_xmlcompiler, reload)
{
  pqrs::xmlcompiler xmlcompiler;
  xmlcompiler.reload();
}

TEST(pqrs_xmlcompiler_symbolmap_keycode, append)
{
  pqrs::xmlcompiler::symbolmap_keycode s;
  EXPECT_TRUE(s.append("KeyCode", "SPACE", 36));
  EXPECT_TRUE(s.append("KeyCode", "VK__AUTOINDEX__BEGIN__", 1024));
  EXPECT_TRUE(s.append("KeyCode", "VK_NEW1"));
  EXPECT_TRUE(s.append("KeyCode", "VK_NEW2"));

  {
    boost::optional<unsigned int> expected = 1024;
    boost::optional<unsigned int> actual = s.get("KeyCode::VK_NEW1");
    EXPECT_EQ(expected, actual);
  }

  {
    boost::optional<unsigned int> expected = 1025;
    boost::optional<unsigned int> actual = s.get("KeyCode::VK_NEW2");
    EXPECT_EQ(expected, actual);
  }
}
