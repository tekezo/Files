#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIDEventDriver
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOHIDEventDriver);

namespace {
int counter_;
}

bool org_pqrs_driver_mykext::start(IOService* provider) {
  bool result = false;
  if (counter_ < 10) {
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
  IOLOG_INFO("dispatchKeyboardEvent 0x%x, 0x%x, 0x%x\n", usagePage, usage, value);
  super::dispatchKeyboardEvent(timeStamp, usagePage, usage, value, options);
}

void org_pqrs_driver_mykext::handleInterruptReport(
    AbsoluteTime timeStamp,
    IOMemoryDescriptor* report,
    IOHIDReportType reportType,
    UInt32 reportID) {
  IOLOG_INFO("handleInterruptReport 0x%x\n", reportID);
  super::handleInterruptReport(timeStamp, report, reportType, reportID);
}
