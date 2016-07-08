#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/hid/IOHIDDevice.h>
END_IOKIT_INCLUDE;

#include "IOLogWrapper.hpp"
#include "VirtualHIDKeyboard.hpp"
#include "VirtualHIDManager.hpp"

#define super IOService
OSDefineMetaClassAndStructors(org_pqrs_driver_VirtualHIDManager, IOService);

bool org_pqrs_driver_VirtualHIDManager::init(OSDictionary* dictionary) {
  keyboard_ = nullptr;
  return super::init(dictionary);
}

bool org_pqrs_driver_VirtualHIDManager::start(IOService* provider) {
  IOLOG_INFO("start\n");

  keyboard_ = OSTypeAlloc(org_pqrs_driver_VirtualHIDKeyboard);
  if (!keyboard_) {
    goto error;
  }
  if (!keyboard_->init(nullptr)) {
    goto error;
  }
  keyboard_->attach(this);
  SInt32 score;
  keyboard_->probe(this, &score);
  keyboard_->start(this);

  return super::start(provider);

error:
  stop(provider);
  return false;
}

void org_pqrs_driver_VirtualHIDManager::stop(IOService* provider) {
  if (keyboard_) {
    keyboard_->detach(this);
    keyboard_->stop(this);
    keyboard_->release();
    keyboard_ = nullptr;
  }
}
