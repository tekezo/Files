#include "hid_system_client.hpp"
#include <csignal>

namespace {
auto global_wait = pqrs::make_thread_wait();
}

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  auto client = std::make_unique<hid_system_client>();
  client->caps_lock_state_changed.connect([](auto&& state) {
    if (!state) {
      std::cout << "caps_lock_state_changed: std::nullopt" << std::endl;
    } else {
      std::cout << "caps_lock_state_changed: " << *state << std::endl;
    }
  });
  client->async_start_caps_lock_check_timer(std::chrono::milliseconds(100));

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  client->async_set_caps_lock_state(true);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  client->async_set_caps_lock_state(true);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  client->async_set_caps_lock_state(false);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // ------------------------------------------------------------

  global_wait->wait_notice();

  // ------------------------------------------------------------

  client = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  std::cout << "finished" << std::endl;

  return 0;
}
