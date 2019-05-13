#include "gtest/gtest.h"

#include <xk/xk.h>
#include <xk/SticksChecker.h>
#include <xk/SticksStore.h>

TEST(SticksStoreTest, A) {
    SticksStore::instance().store.clear();
    EXPECT_EQ(SticksStore::instance().store.size(), 0);

    Time beginTime;
    Time endTime("2019-05-13 16:00");
    Time endTime2("2019-05-13 17:00");
    beginTime.AddDay(-200);
    MChart mc("XAUUSD", beginTime, endTime);

    EXPECT_TRUE(SticksStore::instance().store.size() > 0);

    SticksChecker checker;
    EXPECT_TRUE(mc.sticks().size() > 150 * 200);
    EXPECT_TRUE(checker.Check(mc.sticks()));

    MChart mc2("XAUUSD", beginTime, endTime2);
    EXPECT_TRUE(mc2.sticks().size() > 150 * 200);
    EXPECT_TRUE(checker.Check(mc2.sticks()));

    EXPECT_EQ(mc.sticks().size() + 12, mc2.sticks().size());

    MChart mc3("XAUUSD", beginTime.AddDay(-100), Time().AddDay(1));
    EXPECT_TRUE(mc3.sticks().size() -  mc.sticks().size() > 80 * 200);
    //EXPECT_EQ(mc3.sticks().back().close_time.ToString(), "2019-05-13 19:25:00");
}

TEST(SticksStoreTest, B) {
    SticksStore::instance().store.clear();
    EXPECT_EQ(SticksStore::instance().store.size(), 0);

    Time beginTime;
    Time endTime2("2019-05-13 16:00");
    Time endTime("2019-05-13 17:00");
    beginTime.AddDay(-200);
    MChart mc("XAUUSD", beginTime, endTime);

    EXPECT_TRUE(SticksStore::instance().store.size() > 0);

    SticksChecker checker;
    EXPECT_TRUE(mc.sticks().size() > 150 * 200);
    EXPECT_TRUE(checker.Check(mc.sticks()));

    MChart mc2("XAUUSD", beginTime, endTime2);
    EXPECT_TRUE(mc2.sticks().size() > 150 * 200);
    EXPECT_TRUE(checker.Check(mc2.sticks()));

    EXPECT_EQ(mc.sticks().size(), mc2.sticks().size() + 12);
}

TEST(SticksStoreTest, C) {
    SticksStore::instance().store.clear();
    EXPECT_EQ(SticksStore::instance().store.size(), 0);

    Time beginTime;
    Time endTime2("2019-05-13 16:00");
    Time endTime("2019-05-13 16:00");
    beginTime.AddDay(-200);
    MChart mc("XAUUSD", beginTime, endTime);

    EXPECT_TRUE(SticksStore::instance().store.size() > 0);

    SticksChecker checker;
    EXPECT_TRUE(mc.sticks().size() > 150 * 60);
    EXPECT_TRUE(checker.Check(mc.sticks()));

    MChart mc2("XAUUSD", beginTime, endTime2);
    EXPECT_TRUE(mc2.sticks().size() > 150 * 60);
    EXPECT_TRUE(checker.Check(mc2.sticks()));

    //EXPECT_EQ(mc.sticks().size(), mc2.sticks().size() + 12);
}

TEST(SticksStoreTest, I) {
    SticksStore::instance().store.clear();
    EXPECT_EQ(SticksStore::instance().store.size(), 0);

    Time beginTime;
    Time endTime("2019-05-13 14:00");
    Time endTime2("2019-05-13 15:00");
    beginTime.AddDay(-200);
    MChart mc("I", beginTime, endTime);

    EXPECT_TRUE(SticksStore::instance().store.size() > 0);

    SticksChecker checker;
    EXPECT_TRUE(mc.sticks().size() > 150 * 60);
    EXPECT_TRUE(checker.Check(mc.sticks()));
    EXPECT_TRUE(checker.CheckTimeDelta(mc.sticks()));

    MChart mc2("I", beginTime, endTime2);
    EXPECT_TRUE(mc2.sticks().size() > 150 * 60);
    EXPECT_TRUE(checker.Check(mc2.sticks()));
    EXPECT_TRUE(checker.CheckTimeDelta(mc2.sticks()));

    EXPECT_EQ(mc.sticks().size() + 12, mc2.sticks().size());
}