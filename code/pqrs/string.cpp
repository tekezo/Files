#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "pqrs/string.hpp"

namespace pqrs {
  namespace string {
    static int
    string_by_replacing_double_curly_braces_(std::string& string, std::istream& istream, replacement replacement)
    {
      int previous = '\0';

      if (! istream.good()) {
        return -1;
      }

      for (;;) {
        int c = istream.get();
        if (! istream.good()) goto finish;

        // ----------------------------------------
        // processing {{ XXX }}
        if (c == '{' && previous == '{') {
          // --------------------
          // remove {
          string.erase(string.length() - 1, 1);

          // --------------------
          // finding }}
          std::string key;

          for (;;) {
            c = istream.get();
            if (! istream.good()) {
              // }} is not found
              string += "{{";
              string += key;
              goto finish;
            }

            if (c == '}' && previous == '}') {
              // remove }
              key.erase(key.length() - 1, 1);
              boost::trim(key);

              replacement::iterator it = replacement.find(key);
              if (it != replacement.end()) {
                string += it->second;
              }
              break;
            }

            key.push_back(c);
            previous = c;
          }

        } else {
          string.push_back(c);
          previous = c;
        }
      }

    finish:
      return 0;
    }

    int
    string_by_replacing_double_curly_braces_from_file(std::string& string,
                                                      const char* filename,
                                                      replacement replacement)
    {
      string.clear();

      // ----------------------------------------
      // Validate parameters.
      if (! filename) {
        return 0;
      }

      // ----------------------------------------
      std::ifstream istream(filename);
      if (! istream) {
        return -1;
      }

      // ----------------------------------------
      // Get length of file and call string.reserve with file length.
      istream.seekg(0, std::ios::end);
      string.reserve(istream.tellg());
      istream.seekg(0, std::ios::beg);

      // ----------------------------------------
      return string_by_replacing_double_curly_braces_(string, istream, replacement);
    }

    int
    string_by_replacing_double_curly_braces_from_string(std::string& string,
                                                        const std::string& source,
                                                        replacement replacement)
    {
      string.clear();

      // ----------------------------------------
      // Validate parameters.
      if (source.empty()) {
        return 0;
      }

      // ----------------------------------------
      std::stringstream istream(source, std::stringstream::in);
      if (! istream) {
        return -1;
      }

      // ----------------------------------------
      string.reserve(source.length());

      // ----------------------------------------
      return string_by_replacing_double_curly_braces_(string, istream, replacement);
    }
  }
}
