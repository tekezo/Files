#pragma once

#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOService.h>
#include <IOKit/hidsystem/IOHIDUsageTables.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
END_IOKIT_INCLUDE;

class org_pqrs_driver_mykext : public IOService {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool init(OSDictionary* dictionary = 0) override;
  virtual void free(void) override;
  virtual IOService* probe(IOService* provider, SInt32* score) override;
  virtual bool start(IOService* provider) override;
  virtual void stop(IOService* provider) override;
};
