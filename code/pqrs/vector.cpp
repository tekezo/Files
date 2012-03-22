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
  }
}
