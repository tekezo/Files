#include <ostream>
#include <gtest/gtest.h>
#include "pqrs/string.hpp"

TEST(pqrs_string, string_by_replacing_double_curly_braces_from_file)
{
  pqrs::string::replacement replacement;
  replacement["AAA"] = "1";
  replacement["BBB"] = "2222";
  replacement["CCC"] = "";

  std::string actual;
  pqrs::string::string_by_replacing_double_curly_braces_from_file(actual, "data/sample", replacement);
  EXPECT_EQ("1 2222 \n", actual);
}

TEST(pqrs_string, string_by_replacing_double_curly_braces_from_string)
{
  pqrs::string::replacement replacement;
  replacement["AAA"] = "1";
  replacement["BBB"] = "2222";
  replacement["CCC"] = "";
  replacement["DDD"] = "44444444444444444444";

  std::string actual;
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "abc", replacement);
  EXPECT_EQ("abc", actual);

  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{AAA}} {{BBB}} {{ CCC }}", replacement);
  EXPECT_EQ("1 2222 ", actual);

  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{AAA} } BBB}} XXX", replacement);
  EXPECT_EQ(" XXX", actual);

  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{AAA}", replacement);
  EXPECT_EQ("{{AAA}", actual);

  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{ AAA }", replacement);
  EXPECT_EQ("{{ AAA }", actual);

  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{DDD}}", replacement);
  EXPECT_EQ("44444444444444444444", actual);
}
