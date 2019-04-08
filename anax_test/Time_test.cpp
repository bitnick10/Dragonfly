#include "gtest/gtest.h"

#include <xk/xk.h>

TEST(TimeTest, A) {
    Time b("2019-04-08_09:01", "%Y-%m-%d_%H:%M");
    EXPECT_EQ(b.ToString(), "2019-04-08 09:01:00");
}
