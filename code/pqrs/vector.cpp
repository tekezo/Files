#include <functional>
#include <algorithm>
#include "pqrs/vector.hpp"

namespace pqrs {
  namespace vector {
    void
    remove_empty_strings(std::vector<std::string>& v)
    {
      auto it = std::remove_if(v.begin(), v.end(), mem_fun_ref(&std::string::empty));
      v.erase(it, v.end());
    }

    void
    push_back(std::vector<uint32_t>& v1, const std::vector<uint32_t>& v2)
    {
      std::copy(v2.begin(), v2.end(), std::back_inserter(v1));
    }
  }
}
