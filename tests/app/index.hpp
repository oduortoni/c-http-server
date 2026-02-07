#include <gtest/gtest.h>

extern "C" {
#include "src/app/header.h"
}

TEST(app_index, wrong_form_data)
{
        struct ResponseWriter rw = {};
        const char* body         = "";
        struct Request r         = {};
        strcpy(r.method, "POST");
        strcpy(r.body, body);
        r.body_length = strlen(body);
        int result    = Index(&rw, &r);
        ASSERT_EQ(result, -1);
}

TEST(app_index, correct_form_data)
{
        struct ResponseWriter rw = {};
        InitResponseWriter(&rw);
        const char* body =
            "name=John Doe"
            "&email=jd@example.com"
            "&message=hello world";
        struct Request r = {};
        strcpy(r.method, "POST");
        strcpy(r.body, body);
        r.body_length = strlen(body);
        int result    = Index(&rw, &r);
        ASSERT_EQ(result, 0);
}
