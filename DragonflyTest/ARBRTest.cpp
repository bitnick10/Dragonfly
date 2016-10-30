#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"
#include "../Dragonfly/ASI.h"

TEST(ARBRTest, Method) {
    Stock stock("600354", "XXXX", TradeDataType::Day, eight_digit_time(20150101));

    ARBRCalculator cal(stock);
    auto arbr = cal.GetResult();
    int i = stock.GetIndex(eight_digit_time(20160125));

    EXPECT_NEAR(arbr[i].ar, 153.1, 0.1);
}
