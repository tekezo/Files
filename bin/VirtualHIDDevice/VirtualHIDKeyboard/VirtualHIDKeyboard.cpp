#include "VirtualHIDKeyboard.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIKeyboard
OSDefineMetaClassAndStructors(org_pqrs_driver_VirtualHIDKeyboard, IOHIKeyboard);

bool org_pqrs_driver_VirtualHIDKeyboard::init(OSDictionary* dictionary = 0) {
  bool res = super::init(dictionary);
  return res;
}

bool org_pqrs_driver_VirtualHIDKeyboard::start(IOService* provider) {
  bool res = super::start(provider);

  IOLOG_INFO("start\n");

  AbsoluteTime ts;
  clock_get_uptime(&ts);

  dispatchKeyboardEvent(0xa0, true, ts);
  dispatchKeyboardEvent(0xa0, false, ts);

  return res;
}
