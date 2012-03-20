#include "pqrs/bridge.h"
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  xmlcompiler::remapclasses_initialize_vector::remapclasses_initialize_vector(void)
  {
    data_.resize(2);
    data_[INDEX_OF_FORMAT_VERSION] = BRIDGE_REMAPCLASS_INITIALIZE_VECTOR_FORMAT_VERSION;
    data_[INDEX_OF_COUNT] = 0;
  }

  void
  xmlcompiler::remapclasses_initialize_vector::add(const std::vector<uint32_t>& v, uint32_t configindex)
  {
    std::copy(v.begin(), v.end(), std::back_inserter(data_));
    ++(data_[INDEX_OF_COUNT]);

    if (configindex > max_configindex_) {
      max_configindex_ = configindex;
    }
    is_configindex_appended_[configindex] = true;
  }
}
