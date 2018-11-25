#include <csignal>
#include <pqrs/osx/iokit_service_monitor.hpp>

namespace {
auto global_wait = pqrs::make_thread_wait();
}

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  std::unique_ptr<pqrs::osx::iokit_service_monitor> service_monitor;

  // auto service_name = "IOHIDInterface"; // kIOReturnUnsupported
  auto service_name = "IOHIDEventDriver"; // kIOReturnSuccess, but no effect.
  // auto service_name = "IOHIDEventServiceUserClient"; // not found
  // auto service_name = "org_pqrs_driver_Karabiner_VirtualHIDDevice_VirtualHIDKeyboard_v061000"; // kIOReturnUnsupported
  // auto service_name = "IOHIDLibUserClient"; // not found

  if (auto matching_dictionary = IOServiceNameMatching(service_name)) {
    service_monitor = std::make_unique<pqrs::osx::iokit_service_monitor>(pqrs::dispatcher::extra::get_shared_dispatcher(),
                                                                         matching_dictionary);

    service_monitor->service_matched.connect([](auto&& registry_entry_id, auto&& service_ptr) {
      std::cout << "service_matched " << registry_entry_id << std::endl;

      int32_t value = 2109;
      if (auto number = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value)) {
        pqrs::osx::iokit_return r = IORegistryEntrySetCFProperty(*service_ptr, CFSTR("CapsLockDelay"), number);
        std::cout << "CapsLockDelay " << r << " " << std::endl;
        CFRelease(number);
      }

      value = 4321;
      if (auto number = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value)) {
        pqrs::osx::iokit_return r = IORegistryEntrySetCFProperty(*service_ptr, CFSTR("CapsLockDelayOverride"), number);
        std::cout << "CapsLockDelayOverride " << r << " " << std::endl;
        CFRelease(number);
      }

      std::cout << std::endl;
    });

    service_monitor->service_terminated.connect([](auto&& registry_entry_id) {
      std::cout << "service_terminated " << registry_entry_id << std::endl;

      std::cout << std::endl;
    });

    service_monitor->async_start();

    CFRelease(matching_dictionary);
  }

  // ------------------------------------------------------------

  global_wait->wait_notice();

  // ------------------------------------------------------------

  service_monitor = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  std::cout << "finished" << std::endl;

  return 0;
}
