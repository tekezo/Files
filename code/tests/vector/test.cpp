#include <ostream>
#include <gtest/gtest.h>
#include "pqrs/vector.hpp"

TEST(pqrs_vector, remove_empty_strings)
{
  std::vector<std::string> v;
  v.push_back("");
  v.push_back("A");
  v.push_back("");
  v.push_back("");
  v.push_back("B");
  v.push_back("");
  v.push_back("C");
  v.push_back("");
  v.push_back("");
  pqrs::vector::remove_empty_strings(v);

  std::vector<std::string> expected;
  expected.push_back("A");
  expected.push_back("B");
  expected.push_back("C");

  EXPECT_EQ(expected, v);
}
