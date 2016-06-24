#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hidevent/IOHIDEventDriver.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOHIDEventDriver {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool start(IOService* provider) override;
};
