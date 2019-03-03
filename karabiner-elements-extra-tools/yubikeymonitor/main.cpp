#include <csignal>
#include <pqrs/cf/run_loop_thread.hpp>
#include <pqrs/osx/iokit_hid_device.hpp>
#include <pqrs/osx/iokit_hid_manager.hpp>
#include <unordered_set>

namespace {
auto global_wait = pqrs::make_thread_wait();
}

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  std::unordered_set<pqrs::osx::iokit_registry_entry_id> yubikey_registry_entry_ids;

  std::vector<pqrs::cf::cf_ptr<CFDictionaryRef>> matching_dictionaries{
      pqrs::osx::iokit_hid_manager::make_matching_dictionary(
          pqrs::osx::iokit_hid_usage_page_generic_desktop,
          pqrs::osx::iokit_hid_usage_generic_desktop_keyboard),
  };

  auto hid_manager = std::make_unique<pqrs::osx::iokit_hid_manager>(pqrs::dispatcher::extra::get_shared_dispatcher(),
                                                                    matching_dictionaries);

  hid_manager->device_matched.connect([&yubikey_registry_entry_ids](auto&& registry_entry_id, auto&& device_ptr) {
    if (device_ptr) {
      pqrs::osx::iokit_hid_device d(*device_ptr);
      if (auto vendor_id = d.find_vendor_id()) {
        if (*vendor_id == pqrs::osx::iokit_hid_vendor_id(0x1050)) {
          yubikey_registry_entry_ids.insert(registry_entry_id);
        }
      }
    }
  });

  hid_manager->device_terminated.connect([&yubikey_registry_entry_ids](auto&& registry_entry_id) {
    auto it = yubikey_registry_entry_ids.find(registry_entry_id);
    if (it != std::end(yubikey_registry_entry_ids)) {
      system("'/System/Library/CoreServices/Menu Extras/User.menu/Contents/Resources/CGSession' -suspend");
      yubikey_registry_entry_ids.erase(registry_entry_id);
    }
  });

  hid_manager->async_start();

  // ------------------------------------------------------------

  global_wait->wait_notice();

  // ------------------------------------------------------------

  hid_manager = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  return 0;
}
