#include <gtest/gtest.h>

extern "C" {
#include "utils/string-builder/header.h"
}

TEST(lib_utils_string_builder, mixed_ends_with_binary)
{
        struct StringBuilder sb = {};
        std::string_view s      = "Hello";
        sb_appendf(&sb, "Today is %s!", "Friday");
        ASSERT_STREQ(sb.ascii.data, "Today is Friday!");
        sb_append_binary(&sb, (unsigned char *)s.data(), s.size());
#define expected "Today is Friday!Hello"
        ASSERT_EQ(memcmp(sb.ascii.data, expected, sizeof(expected) - 1), 0);
        free(sb.ascii.data);
}
