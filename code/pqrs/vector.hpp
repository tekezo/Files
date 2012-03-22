#ifndef PQRS_VECTOR_HPP
#define PQRS_VECTOR_HPP

#include <vector>
#include <string>

namespace pqrs {
  namespace vector {
    void remove_empty_strings(std::vector<std::string>& v);
    void push_back(std::vector<uint32_t>& v1, const std::vector<uint32_t>& v2);
  }
}

#endif
