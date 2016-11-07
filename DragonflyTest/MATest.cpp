#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"
#include "../Dragonfly/ASI.h"

TEST(MATest, Method) {
    Stock stock("000002", "WANKE", TradeDataType::Day, eight_digit_time(20150101));

    //ARBRCalculator cal(stock);
    //auto arbr = cal.GetResult();
    int i = stock.GetIndex(eight_digit_time(20161031));

    EXPECT_NEAR(stock.indicators().ma5()[i], 24.92, 0.01);
    EXPECT_NEAR(stock.indicators().ma10()[i], 25.29, 0.01);
    EXPECT_NEAR(stock.indicators().ma20()[i], 25.72, 0.01);
    EXPECT_NEAR(stock.indicators().ma60()[i], 23.93, 0.01);
}
