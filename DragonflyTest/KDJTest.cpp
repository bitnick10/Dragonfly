#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"
#include "../Dragonfly/ASI.h"

TEST(KDJTest, Method) {
    Stock stock("000002", "WANKE", TradeDataType::Day, eight_digit_time(20150101));

    //ARBRCalculator cal(stock);
    //auto arbr = cal.GetResult();
    int i = stock.GetIndex(eight_digit_time(20161101));

    // EXPECT_NEAR(stock.indicators().kdj()[i].k, 44.27, 0.01);
    //EXPECT_NEAR(stock.indicators().kdj()[i].d, 29.38, 0.01);
    EXPECT_TRUE(stock.indicators().kdj()[i].k > stock.indicators().kdj()[i].d);
    EXPECT_NEAR(stock.indicators().kdj()[i].j, 74.04, 0.1);

}
