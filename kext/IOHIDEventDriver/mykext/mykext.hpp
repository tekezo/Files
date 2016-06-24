#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hidevent/IOHIDEventDriver.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOHIDEventDriver {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool start(IOService* provider) override;

  virtual void dispatchKeyboardEvent(
      AbsoluteTime timeStamp,
      UInt32 usagePage,
      UInt32 usage,
      UInt32 value,
      IOOptionBits options = 0) override;

  virtual void handleInterruptReport(
      AbsoluteTime timeStamp,
      IOMemoryDescriptor* report,
      IOHIDReportType reportType,
      UInt32 reportID) override;
};
