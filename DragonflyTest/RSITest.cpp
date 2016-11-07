#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"
#include "../Dragonfly/RSI.h"

TEST(RSITest, Method) {
    Stock stock("000002", "WANKE", TradeDataType::Day, eight_digit_time(20150101));

    RSICalculator cal(stock, 5);
    auto rsi = cal.GetResult();
    int i = stock.GetIndex(eight_digit_time(20161101));

    //EXPECT_NEAR(stock.indicators().rsi6()[i], 63.42, 0.01);

    EXPECT_NEAR(cal.ups[i - 0], 0.87, 0.01);  // 25.79 - 24.92 = 0.87
    EXPECT_NEAR(cal.ups[i - 1], 0.17, 0.01); // 24.92 - 24.75 =
    EXPECT_NEAR(cal.ups[i - 2], 0, 0.01); //24.75 - 24.77
    EXPECT_NEAR(cal.ups[i - 3], 0, 0.01); //24.77 - 25.17
    EXPECT_NEAR(cal.ups[i - 4], 0.16, 0.01); //25.17 - 25.01
    EXPECT_NEAR(cal.ups[i - 5], 0, 0.01); //25.01 - 25.24

    EXPECT_NEAR(cal.downs[i - 0], 0.87, 0.01);
    EXPECT_NEAR(cal.downs[i - 1], 0.17, 0.01);
    EXPECT_NEAR(cal.downs[i - 2], 0.02, 0.01);
    EXPECT_NEAR(cal.downs[i - 3], 0.4, 0.01);
    EXPECT_NEAR(cal.downs[i - 4], 0.16, 0.01);
    EXPECT_NEAR(cal.downs[i - 5], 0.23, 0.01);

    EXPECT_NEAR(rsi[i], 63.42, 0.1);
    EXPECT_NEAR(rsi[i - 1], 36.00, 0.1);
    EXPECT_NEAR(rsi[i - 2], 27.10, 0.1);

    int i2 = stock.GetIndex(eight_digit_time(20160922));
    EXPECT_NEAR(rsi[i2], 88.65, 0.1);
}
