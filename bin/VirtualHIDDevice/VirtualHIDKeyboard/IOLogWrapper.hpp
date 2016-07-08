#pragma once

#define IOLOG_INFO(...)                                                \
  {                                                                    \
    IOLog("org.pqrs.driver.VirtualHIDKeyboard --Info-- " __VA_ARGS__); \
  }
