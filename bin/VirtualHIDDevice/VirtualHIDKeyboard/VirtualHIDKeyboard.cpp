#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
END_IOKIT_INCLUDE;

#include "IOLogWrapper.hpp"
#include "VirtualHIDKeyboard.hpp"

#define super IOHIKeyboard
OSDefineMetaClassAndStructors(org_pqrs_driver_VirtualHIDKeyboard, IOHIKeyboard);

bool org_pqrs_driver_VirtualHIDKeyboard::start(IOService* provider) {
  IOLOG_INFO("start\n");

  // set kIOHIDDeviceUsagePageKey
  {
    OSNumber* usagePage = OSNumber::withNumber(kHIDPage_GenericDesktop, 32);
    if (usagePage) {
      setProperty(kIOHIDDeviceUsagePageKey, usagePage);
      usagePage->release();
    }
  }

  // set kIOHIDDeviceUsageKey
  {
    OSNumber* usage = OSNumber::withNumber(kHIDUsage_GD_Keyboard, 32);
    if (usage) {
      setProperty(kIOHIDDeviceUsageKey, usage);
      usage->release();
    }
  }

  setProperty(kIOHIDVirtualHIDevice, kOSBooleanTrue);

  return super::start(provider);
}
