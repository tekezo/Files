#pragma once

class VirtualHIDKeyboard;

class org_pqrs_driver_VirtualHIDManager final : public IOService {
  OSDeclareDefaultStructors(org_pqrs_driver_VirtualHIDManager);

public:
  virtual bool init(OSDictionary* dictionary = 0) override;
  virtual bool start(IOService* provider) override;
  virtual void stop(IOService* provider) override;

private:
  org_pqrs_driver_VirtualHIDKeyboard* keyboard_;
};
