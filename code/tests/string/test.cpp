#include <ostream>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>
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

  // performance test
  {
    for (int i = 0; i < 1000; ++i) {
      replacement[std::string("TARGET") + boost::lexical_cast<std::string>(i)] = "REPLACEMENT";
    }
    const char* filepath = "/Library/org.pqrs/KeyRemap4MacBook/prefpane/checkbox.xml";
    pqrs::string::string_by_replacing_double_curly_braces_from_file(actual, filepath, replacement);
  }
}

TEST(pqrs_string, string_by_replacing_double_curly_braces_from_string)
{
  pqrs::string::replacement replacement;
  replacement["AAA"] = "1";
  replacement["BBB"] = "2222";
  replacement["CCC"] = "";
  replacement["DDD"] = "44444444444444444444";
  replacement["LOOP1"] = "{{ LOOP1 }}";
  replacement["LOOP2"] = "   {{ LOOP2 }}    ";

  std::string actual;

  // no replacing
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "abc", replacement);
  EXPECT_EQ("abc", actual);

  // normal replacing
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{AAA}} {{BBB}} !{{ CCC }}!{{ DDD }}", replacement);
  EXPECT_EQ("1 2222 !!44444444444444444444", actual);

  // unknown replacing
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{UNKNOWN}}", replacement);
  EXPECT_EQ("", actual);

  // "} }" is not end.
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{AAA} } BBB}} XXX", replacement);
  EXPECT_EQ(" XXX", actual);

  // no }}
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{AAA}", replacement);
  EXPECT_EQ("{{AAA}", actual);

  // no }}
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{ AAA }", replacement);
  EXPECT_EQ("{{ AAA }", actual);

  // looped replacing
  pqrs::string::string_by_replacing_double_curly_braces_from_string(actual, "{{ LOOP1 }}{{ LOOP2 }}", replacement);
  EXPECT_EQ("{{ LOOP1 }}   {{ LOOP2 }}    ", actual);
}

TEST(pqrs_string, to_uint32_t)
{
  boost::optional<uint32_t> actual;

  actual = pqrs::string::to_uint32_t("123456");
  EXPECT_TRUE(actual);
  EXPECT_EQ(123456, *actual);

  // oct
  actual = pqrs::string::to_uint32_t("0100");
  EXPECT_TRUE(actual);
  EXPECT_EQ(64, *actual);

  // hex
  actual = pqrs::string::to_uint32_t("0x123456");
  EXPECT_TRUE(actual);
  EXPECT_EQ(1193046, *actual);

  actual = pqrs::string::to_uint32_t("0xG");
  EXPECT_FALSE(actual);

  actual = pqrs::string::to_uint32_t("abc");
  EXPECT_FALSE(actual);
}
