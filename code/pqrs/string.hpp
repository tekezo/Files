#ifndef PQRS_STRING_HPP
#define PQRS_STRING_HPP

#include <string>
#include <tr1/unordered_map>

namespace pqrs {
  namespace string {
    typedef std::tr1::unordered_map<std::string, std::string> replacement;

    int string_by_replacing_double_curly_braces_from_file(std::string& string,
                                                          const char* filename,
                                                          replacement replacement);
    int string_by_replacing_double_curly_braces_from_string(std::string& string,
                                                            const std::string& source,
                                                            replacement replacement);
  }
}

#endif
