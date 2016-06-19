#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOService
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOService);

bool org_pqrs_driver_mykext::init(OSDictionary* dict) {
  IOLOG_INFO("init\n");

  bool res = super::init(dict);

  matchedNotifier_ = nullptr;
  terminatedNotifier_ = nullptr;

  return res;
}

void org_pqrs_driver_mykext::free(void) {
  IOLOG_INFO("free\n");

  super::free();
}

IOService* org_pqrs_driver_mykext::probe(IOService* provider, SInt32* score) {
  IOService* res = super::probe(provider, score);
  return res;
}

bool org_pqrs_driver_mykext::start(IOService* provider) {
  IOLOG_INFO("start\n");

  bool res = super::start(provider);
  if (!res) { return res; }

  matchedNotifier_ = addMatchingNotification(gIOMatchedNotification,
                                             serviceMatching("IOHIDEventDriver"),
                                             org_pqrs_driver_mykext::gIOMatchedNotification_callback,
                                             this, nullptr, 0);
  if (matchedNotifier_ == nullptr) {
    IOLOG_ERROR("matchedNotifier_ == nullptr\n");
    return false;
  }

  terminatedNotifier_ = addMatchingNotification(gIOTerminatedNotification,
                                                serviceMatching("IOHIDEventDriver"),
                                                org_pqrs_driver_mykext::gIOTerminatedNotification_callback,
                                                this, nullptr, 0);
  if (terminatedNotifier_ == nullptr) {
    IOLOG_ERROR("terminatedNotifier_ == nullptr\n");
    return false;
  }

  return res;
}

void org_pqrs_driver_mykext::stop(IOService* provider) {
  IOLOG_INFO("stop\n");

  if (matchedNotifier_) {
    matchedNotifier_->remove();
  }
  if (terminatedNotifier_) {
    terminatedNotifier_->remove();
  }

  super::stop(provider);
}

bool org_pqrs_driver_mykext::gIOMatchedNotification_callback(void* target, void* refCon, IOService* newService, IONotifier* notifier) {
  IOLOG_INFO("gIOMatchedNotification_callback %p\n", newService);
  return true;
}

bool org_pqrs_driver_mykext::gIOTerminatedNotification_callback(void* target, void* refCon, IOService* newService, IONotifier* notifier) {
  IOLOG_INFO("gIOTerminatedNotification_callback %p\n", newService);
  return true;
}
