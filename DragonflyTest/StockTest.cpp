#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"

TEST(StockTest, Method) {
    Stock stock("000418", "XXXX", TradeDataType::Day, eight_digit_time(20150101));

    //ARBRCalculator cal(stock);
    //auto arbr = cal.GetResult();
    int i = stock.GetIndex(eight_digit_time(20160912));
    //stock.trade_data()[i].open_percent()
    //EXPECT_NEAR(stock.trade_data()[i].open_percent(), 2.3, 0.1);
}
