#include "gtest/gtest.h"

#include <xk/xk.h>

TEST(ChartTest, A) {
    Time beginTime("2018-01-01");
    MChart mc("I", RAChart::Period::M5, beginTime, Time());

    EXPECT_EQ(mc.GetIndex(Time("2019-04-19 15:01")), -1);
    EXPECT_EQ(mc.GetIndex(mc.sticks()[0].close_time), 0);
    EXPECT_EQ(mc.GetIndex(mc.sticks().back().close_time), mc.sticks().size() - 1);
    EXPECT_EQ(mc.GetIndex(Time("2019-01-01 15:01")), -1);
    EXPECT_EQ(mc.GetIndex(Time("2019-04-19 15:01")), -1);
    EXPECT_TRUE(mc.GetIndex(Time("2019-04-18 15:00")) != -1);
    EXPECT_EQ(mc.sticks()[mc.GetIndex(Time("2019-04-19 15:00"))].close_time, Time("2019-04-19 15:00"));

}
