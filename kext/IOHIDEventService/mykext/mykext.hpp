#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hidevent/IOHIDEventService.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOHIDEventService {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);
};
