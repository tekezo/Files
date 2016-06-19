#include "mykext.hpp"
#include "IOLogWrapper.hpp"

#define super IOService
OSDefineMetaClassAndStructors(org_pqrs_driver_mykext, IOService);

bool org_pqrs_driver_mykext::init(OSDictionary* dict) {
  IOLOG_INFO("init\n");

  bool res = super::init(dict);
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
  return res;
}

void org_pqrs_driver_mykext::stop(IOService* provider) {
  IOLOG_INFO("stop\n");

  super::stop(provider);
}
