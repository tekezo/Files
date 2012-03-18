#include <gtest/gtest.h>
#include "pqrs/xmlcompiler.hpp"

TEST(pqrs_xmlcompiler, reload)
{
  pqrs::xmlcompiler xmlcompiler;
  xmlcompiler.reload();
}
