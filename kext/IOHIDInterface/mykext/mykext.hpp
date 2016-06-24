#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
#include <IOKit/hid/IOHIDInterface.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOHIDInterface {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool start(IOService* provider) override;

  virtual void handleReport(
      AbsoluteTime timeStamp,
      IOMemoryDescriptor* report,
      IOHIDReportType reportType,
      UInt32 reportID,
      IOOptionBits options = 0) override;
};
