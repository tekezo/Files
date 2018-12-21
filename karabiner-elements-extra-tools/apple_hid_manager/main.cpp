#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDManager.h>
#include <iostream>
#include <pqrs/cf/number.hpp>
#include <pqrs/cf/run_loop_thread.hpp>
#include <pqrs/osx/iokit_return.hpp>

namespace {
static void device_matching_callback(void* _Nullable context,
                                     IOReturn result,
                                     void* _Nullable sender,
                                     IOHIDDeviceRef _Nonnull device) {
  std::cout << "m" << std::flush;
}
} // namespace

int main(int argc, const char* argv[]) {
  auto cf_run_loop_thread = std::make_unique<pqrs::cf::run_loop_thread>();

  for (int i = 0; i < 10000; ++i) {
    std::cout << "." << std::flush;

    auto manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    if (auto matching_dictionaries = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks)) {
      if (auto matching_dictionary = IOServiceMatching(kIOHIDDeviceKey)) {
        if (auto number = pqrs::cf::make_cf_number(static_cast<int32_t>(kHIDPage_GenericDesktop))) {
          CFDictionarySetValue(matching_dictionary,
                               CFSTR(kIOHIDDeviceUsagePageKey),
                               *number);
        }
        if (auto number = pqrs::cf::make_cf_number(static_cast<int32_t>(kHIDUsage_GD_Keyboard))) {
          CFDictionarySetValue(matching_dictionary,
                               CFSTR(kIOHIDDeviceUsageKey),
                               *number);
        }

        CFArrayAppendValue(matching_dictionaries, matching_dictionary);

        CFRelease(matching_dictionary);
      }

      IOHIDManagerSetDeviceMatchingMultiple(manager, matching_dictionaries);

      CFRelease(matching_dictionaries);
    }

    IOHIDManagerRegisterDeviceMatchingCallback(manager, device_matching_callback, nullptr);
    IOHIDManagerScheduleWithRunLoop(manager,
                                    cf_run_loop_thread->get_run_loop(),
                                    kCFRunLoopCommonModes);
    IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    IOHIDManagerClose(manager, kIOHIDOptionsTypeNone);
    IOHIDManagerUnscheduleFromRunLoop(manager,
                                      cf_run_loop_thread->get_run_loop(),
                                      kCFRunLoopCommonModes);

    CFRelease(manager);
  }

  cf_run_loop_thread->terminate();
  cf_run_loop_thread = nullptr;

  return 0;
}
