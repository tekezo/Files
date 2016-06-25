#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#define private public
#include <IOKit/hidsystem/IOHIDSystem.h>
#undef private

#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOHIDDevice {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool start(IOService* provider) override;

  virtual IOReturn newReportDescriptor(IOMemoryDescriptor** descriptor) const override;

  virtual IOReturn handleReportWithTime(
      AbsoluteTime timeStamp,
      IOMemoryDescriptor* report,
      IOHIDReportType reportType = kIOHIDReportTypeInput,
      IOOptionBits options = 0) override;
};
