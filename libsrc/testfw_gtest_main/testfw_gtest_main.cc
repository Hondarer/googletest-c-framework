#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gtest/gtest.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

using namespace testing;

// main() を持たないテストプログラムのエントリーポイント
int main(int argc, char **argv)
{
  printf("Running main() from %s\n", __FILE__);
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
