#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIDDevice
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOHIDDevice);

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

IOReturn org_pqrs_driver_mykext::newReportDescriptor(IOMemoryDescriptor** descriptor) const {
  return kIOReturnError;
}

IOReturn org_pqrs_driver_mykext::handleReportWithTime(
    AbsoluteTime timeStamp,
    IOMemoryDescriptor* report,
    IOHIDReportType reportType,
    IOOptionBits options) {
  IOLOG_INFO("handleReportWithTime 0x%x\n", reportType);
  return super::handleReportWithTime(timeStamp, report, reportType, options);
}
