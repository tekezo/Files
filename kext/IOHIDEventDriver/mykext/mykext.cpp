#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIDEventDriver
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOHIDEventDriver);

namespace {
int counter_;
}

bool org_pqrs_driver_mykext::start(IOService* provider) {
  bool result = false;
  if (counter_ == 0) {
    result = true;
  }
  IOLOG_INFO("start %d\n", ++counter_);
  ++counter_;
  return result;
}
