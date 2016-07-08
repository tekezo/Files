#pragma once

class org_pqrs_driver_VirtualHIDKeyboard : public IOHIKeyboard {
  OSDeclareDefaultStructors(org_pqrs_driver_VirtualHIDKeyboard);

public:
  virtual bool start(IOService* provider) override;
};
