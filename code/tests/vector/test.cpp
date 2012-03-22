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

TEST(pqrs_vector, make_combination)
{
  std::vector<std::string> seeds;
  seeds.push_back("A");
  seeds.push_back("B");
  seeds.push_back("C");
  seeds.push_back("D");
  std::vector<std::tr1::shared_ptr<std::vector<std::string> > > actual;
  pqrs::vector::make_combination(actual, seeds);

  EXPECT_EQ(16, actual.size());

  { // 0
    std::vector<std::string> expected;
    EXPECT_EQ(expected, *(actual[0]));
  }
  { // 1
    std::vector<std::string> expected;
    expected.push_back("A");
    EXPECT_EQ(expected, *(actual[1]));
  }
  { // 2
    std::vector<std::string> expected;
    expected.push_back("B");
    EXPECT_EQ(expected, *(actual[2]));
  }
  { // 3
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("B");
    EXPECT_EQ(expected, *(actual[3]));
  }
  { // 4
    std::vector<std::string> expected;
    expected.push_back("C");
    EXPECT_EQ(expected, *(actual[4]));
  }
  { // 5
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("C");
    EXPECT_EQ(expected, *(actual[5]));
  }
  { // 6
    std::vector<std::string> expected;
    expected.push_back("B");
    expected.push_back("C");
    EXPECT_EQ(expected, *(actual[6]));
  }
  { // 7
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("B");
    expected.push_back("C");
    EXPECT_EQ(expected, *(actual[7]));
  }
  { // 8
    std::vector<std::string> expected;
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[8]));
  }
  { // 9
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[9]));
  }
  { // 10
    std::vector<std::string> expected;
    expected.push_back("B");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[10]));
  }
  { // 11
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("B");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[11]));
  }
  { // 12
    std::vector<std::string> expected;
    expected.push_back("C");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[12]));
  }
  { // 13
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("C");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[13]));
  }
  { // 14
    std::vector<std::string> expected;
    expected.push_back("B");
    expected.push_back("C");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[14]));
  }
  { // 15
    std::vector<std::string> expected;
    expected.push_back("A");
    expected.push_back("B");
    expected.push_back("C");
    expected.push_back("D");
    EXPECT_EQ(expected, *(actual[15]));
  }
}
