#include "VirtualHIDKeyboard.hpp"
#include "IOLogWrapper.hpp"

#define super IOHIKeyboard
OSDefineMetaClassAndStructors(org_pqrs_driver_VirtualHIDKeyboard, IOHIKeyboard);

bool org_pqrs_driver_VirtualHIDKeyboard::start(IOService* provider) {
    bool res = super::start(provider);
    return res;
}
