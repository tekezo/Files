#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hidevent/IOHIDEventDriver.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_VirtualHIDKeyboard : public IOHIDEventDriver {
  OSDeclareDefaultStructors(org_pqrs_driver_VirtualHIDKeyboard);

public:
  virtual bool start(IOService* provider) override;
};
