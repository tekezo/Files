#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIDInterface
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOHIDInterface);

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

void org_pqrs_driver_mykext::handleReport(
    AbsoluteTime timeStamp,
    IOMemoryDescriptor* report,
    IOHIDReportType reportType,
    UInt32 reportID,
    IOOptionBits options = 0) {
  IOLOG_INFO("handleReport 0x%x\n", reportID);
  super::handleReport(timeStamp, report, reportType, reportID, options);
}
