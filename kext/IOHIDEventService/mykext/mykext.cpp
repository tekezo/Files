#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIDEventService
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOHIDEventService);

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

void org_pqrs_driver_mykext::dispatchKeyboardEvent(
    AbsoluteTime timeStamp,
    UInt32 usagePage,
    UInt32 usage,
    UInt32 value,
    IOOptionBits options) {
  IOLOG_INFO("dispatchKeyboardEvent 0x%x, 0x%x, 0x%x", usagePage, usage, value);
  super::dispatchKeyboardEvent(timeStamp, usagePage, usage, value, options);
}
