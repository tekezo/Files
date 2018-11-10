#include <pqrs/dispatcher.hpp>
#include <pqrs/osx/iokit/ioservice_monitor.hpp>

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::unique_ptr<pqrs::osx::iokit::ioservice_monitor> ioservice_monitor;

  // auto service_name = "IOHIDInterface"; // kIOReturnUnsupported
  // auto service_name = "IOHIDEventDriver"; // kIOReturnSuccess, but no effect.
  auto service_name = "IOHIDEventServiceUserClient"; // not found
  // auto service_name = "org_pqrs_driver_Karabiner_VirtualHIDDevice_VirtualHIDKeyboard_v061000"; // kIOReturnUnsupported
  // auto service_name = "IOHIDLibUserClient"; // not found

  if (auto matching_dictionary = IOServiceNameMatching(service_name)) {
    ioservice_monitor = std::make_unique<pqrs::osx::iokit::ioservice_monitor>(pqrs::dispatcher::extra::get_shared_dispatcher(),
                                                                              matching_dictionary);

    ioservice_monitor->ioservice_detected.connect([](auto&& registry_entry_id, auto&& service) {
      std::cout << "ioservice_detected " << registry_entry_id << std::endl;

      int32_t value = 2109;
      if (auto number = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value)) {
        auto kr = IORegistryEntrySetCFProperty(service, CFSTR("CapsLockDelay"), number);
        std::cout << "kr " << kr << " " << std::endl;
        CFRelease(number);
      }

      value = 4321;
      if (auto number = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value)) {
        auto kr = IORegistryEntrySetCFProperty(service, CFSTR("CapsLockDelayOverride"), number);
        std::cout << "kr " << kr << " " << std::endl;
        CFRelease(number);
      }
    });

    ioservice_monitor->ioservice_removed.connect([](auto&& registry_entry_id) {
      std::cout << "ioservice_removed " << registry_entry_id << std::endl;
    });

    ioservice_monitor->async_start();

    CFRelease(matching_dictionary);
  }

  // ------------------------------------------------------------

  CFRunLoopRun();

  // ------------------------------------------------------------

  ioservice_monitor = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  return 0;
}
