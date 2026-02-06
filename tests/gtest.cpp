#include <gtest/gtest.h>

#include "./lib/utils/string-builder/tests.hpp"

int
main()
{
        testing::InitGoogleTest();
        [[maybe_unused]]
        int ok = RUN_ALL_TESTS();
}
