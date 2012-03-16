#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include "StringFunctions.hpp"

namespace StringFunctions {
  static int
  stringByReplacingDoubleCurlyBraces_(std::string& string, std::istream& istream, Replacement replacement)
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
            string += key;
            goto finish;
          }

          if (! isspace(c)) {
            key.push_back(c);

            if (c == '}' && previous == '}') {
              // remove }}
              key.erase(key.length() - 2, 2);

              Replacement::iterator it = replacement.find(key);
              if (it != replacement.end()) {
                string += it->second;
              }
              break;
            }
          }

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
  stringByReplacingDoubleCurlyBracesFromFile(std::string& string,
                                             const char* filename,
                                             Replacement replacement)
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
    return stringByReplacingDoubleCurlyBraces_(string, istream, replacement);
  }

  int
  stringByReplacingDoubleCurlyBracesFromString(std::string& string,
                                               const std::string& source,
                                               Replacement replacement)
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
    return stringByReplacingDoubleCurlyBraces_(string, istream, replacement);
  }
}
