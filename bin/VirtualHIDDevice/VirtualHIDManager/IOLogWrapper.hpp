#pragma once

#define IOLOG_INFO(...)                                               \
  {                                                                   \
    IOLog("org.pqrs.driver.VirtualHIDManager --Info-- " __VA_ARGS__); \
  }
