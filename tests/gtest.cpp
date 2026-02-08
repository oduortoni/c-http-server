#include <gtest/gtest.h>

#include "./app/index.hpp"
#include "./lib/utils/string-builder/tests.hpp"

int
main(int argc, char **argv)
{
        testing::InitGoogleTest(&argc, argv);
        [[maybe_unused]]
        int ok = RUN_ALL_TESTS();
}
