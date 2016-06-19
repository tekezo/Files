#pragma once

#include "diagnostic_macros.hpp"

#define private public /* A hack for access private member of IOHIKeyboard */
#define protected public /* A hack for access private member of IOHIKeyboard */
BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
#include <IOKit/hidevent/IOHIDEventDriver.h>
#include <IOKit/hidsystem/IOHIDUsageTables.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
#include <IOKit/hidsystem/IOHIPointing.h>
#include "IOHIDKeyboard.h"
END_IOKIT_INCLUDE;
#undef protected

class org_pqrs_driver_mykext : public IOService {
  OSDeclareDefaultStructors(org_pqrs_driver_mykext);

public:
  virtual bool init(OSDictionary* dictionary = 0) override;
  virtual void free(void) override;
  virtual IOService* probe(IOService* provider, SInt32* score) override;
  virtual bool start(IOService* provider) override;
  virtual void stop(IOService* provider) override;

  static bool gIOMatchedNotification_callback(void* target, void* refCon, IOService* newService, IONotifier* notifier);
  static bool gIOTerminatedNotification_callback(void* target, void* refCon, IOService* newService, IONotifier* notifier);

private:
  IONotifier* matchedNotifier_;
  IONotifier* terminatedNotifier_;
};
