// How to build and run this program.
//
// $ c++ -std=c++17 main.cpp -framework SystemConfiguration -framework CoreFoundation
// $ ./a.out

#include <SystemConfiguration/SystemConfiguration.h>
#include <iostream>

int main(int argc, const char* argv[]) {
  uid_t uid = 0;
  gid_t gid = 0;

  if (auto cf_name = SCDynamicStoreCopyConsoleUser(nullptr, &uid, &gid)) {
    std::cout << "uid: " << uid << std::endl;
    std::cout << "gid: " << gid << std::endl;
    CFRelease(cf_name);
  } else {
    std::cout << "SCDynamicStoreCopyConsoleUser returns nullptr" << std::endl;
  }

  return 0;
}
