#include <iostream>

int
main(void)
{
  uint32_t i = (1 << 31);
  std::cout << std::hex << i << std::endl;
  return 0;
}
