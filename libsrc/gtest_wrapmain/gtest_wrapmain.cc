#include "gtest/gtest.h"
#include <gtest_wrapmain.h>

using namespace testing;

// -Wl,--wrap=main を利用して main() を wrap した際のエントリーポイント
int __wrap_main(int argc, char **argv)
{
  printf("Running main() from %s\n", __FILE__);
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
