#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hidevent/IOHIDEventService.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOHIDEventService {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool start(IOService* provider) override;

  virtual void dispatchKeyboardEvent(
      AbsoluteTime timeStamp,
      UInt32 usagePage,
      UInt32 usage,
      UInt32 value,
      IOOptionBits options = 0) override;
};
