#include "iopm_monitor.hpp"
#include <csignal>

namespace {
auto global_wait = pqrs::make_thread_wait();
}

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  auto monitor = std::make_unique<iopm_monitor>();

  monitor->system_power_event_arrived.connect([](auto&& message_type) {
    std::cout << "callback message_type: " << message_type << std::endl;
  });

  monitor->async_start();

  // ------------------------------------------------------------

  global_wait->wait_notice();

  // ------------------------------------------------------------

  monitor = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  std::cout << "finished" << std::endl;

  return 0;
}
