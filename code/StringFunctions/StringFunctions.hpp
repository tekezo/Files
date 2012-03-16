#ifndef STRINGFUNCTIONS_HPP
#define STRINGFUNCTIONS_HPP

#include <string>
#include <vector>
#include <tr1/unordered_map>

namespace StringFunctions {
  typedef std::tr1::unordered_map<std::string, std::string> Replacement;

  int stringByReplacingDoubleCurlyBracesFromFile(std::string& string, const char* filename, Replacement replacement);
  int stringByReplacingDoubleCurlyBracesFromString(std::string& string, const std::string& source, Replacement replacement);
}

#endif
