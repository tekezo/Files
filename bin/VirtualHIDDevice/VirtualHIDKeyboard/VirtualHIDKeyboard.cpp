#include "VirtualHIDKeyboard.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIDEventDriver
OSDefineMetaClassAndStructors(org_pqrs_driver_VirtualHIDKeyboard, IOHIDEventDriver);

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
