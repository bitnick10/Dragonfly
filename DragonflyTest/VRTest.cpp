#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"

TEST(VRTest, Method) {
    Stock stock("600354", "XXXX", TradeDataType::Day, eight_digit_time(20140101));

    int i = stock.GetIndex(eight_digit_time(20160125));
    EXPECT_NEAR(stock.indicators().vr()[i], 142.16, 0.01);
}
