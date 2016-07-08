#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_VirtualHIDKeyboard : public IOHIKeyboard {
  OSDeclareDefaultStructors(org_pqrs_driver_VirtualHIDKeyboard);

public:
  virtual bool init(OSDictionary* dictionary = 0) override;
  virtual bool start(IOService* provider) override;
};
