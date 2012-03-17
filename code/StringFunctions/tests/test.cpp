#include <ostream>
#include <gtest/gtest.h>
#include "StringFunctions.hpp"

TEST(StringFunctions, stringByReplacingDoubleCurlyBracesFromFile)
{
  StringFunctions::Replacement replacement;
  replacement["AAA"] = "1";
  replacement["BBB"] = "2222";
  replacement["CCC"] = "";

  std::string actual;
  StringFunctions::stringByReplacingDoubleCurlyBracesFromFile(actual, "data/sample", replacement);
  EXPECT_EQ("1 2222 \n", actual);
}

TEST(StringFunctions, stringByReplacingDoubleCurlyBracesFromString)
{
  StringFunctions::Replacement replacement;
  replacement["AAA"] = "1";
  replacement["BBB"] = "2222";
  replacement["CCC"] = "";
  replacement["DDD"] = "44444444444444444444";

  std::string actual;
  StringFunctions::stringByReplacingDoubleCurlyBracesFromString(actual, "abc", replacement);
  EXPECT_EQ("abc", actual);

  StringFunctions::stringByReplacingDoubleCurlyBracesFromString(actual, "{{AAA}} {{BBB}} {{ CCC }}", replacement);
  EXPECT_EQ("1 2222 ", actual);

  StringFunctions::stringByReplacingDoubleCurlyBracesFromString(actual, "{{AAA} } BBB}} XXX", replacement);
  EXPECT_EQ(" XXX", actual);

  StringFunctions::stringByReplacingDoubleCurlyBracesFromString(actual, "{{AAA}", replacement);
  EXPECT_EQ("{{AAA}", actual);

  StringFunctions::stringByReplacingDoubleCurlyBracesFromString(actual, "{{DDD}}", replacement);
  EXPECT_EQ("44444444444444444444", actual);
}
