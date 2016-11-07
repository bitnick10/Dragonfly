//#include "gtest/gtest.h"
//
//#include "../Dragonfly/Stock.h"
//#include "../Dragonfly/Market.h"
//
//TEST(StockTest, Method) {
//    Stock stock("000418", "XXXX", TradeDataType::Day, eight_digit_time(20150101));
//
//    //ARBRCalculator cal(stock);
//    //auto arbr = cal.GetResult();
//    int i = stock.GetIndex(eight_digit_time(20160912));
//    //stock.trade_data()[i].open_percent()
//    //EXPECT_NEAR(stock.trade_data()[i].open_percent(), 2.3, 0.1);
//}
//
//TEST(StockTest, is_suspended) {
//    Stock stock("000029", "XXXX", TradeDataType::Day, eight_digit_time(20150101));
//    Stock index("index000001", "XXXX", TradeDataType::Day, eight_digit_time(20150101));
//
//    EXPECT_EQ(stock.GetIndex(eight_digit_time(20160914)), -1);
//    EXPECT_EQ(stock.GetLatestTradingTime(eight_digit_time(20160914)), (year_month_day_hour_min_sec)eight_digit_time(20160913));
//
//    EXPECT_TRUE(stock.is_suspended(eight_digit_time(20160914), index));
//    EXPECT_FALSE(stock.is_suspended(eight_digit_time(20160913), index));
//    EXPECT_FALSE(stock.is_suspended(eight_digit_time(20160912), index));
//    EXPECT_FALSE(stock.is_suspended(eight_digit_time(20160911), index));
//    EXPECT_FALSE(stock.is_suspended(eight_digit_time(20160910), index));
//    EXPECT_FALSE(stock.is_suspended(eight_digit_time(20160909), index));
//    EXPECT_FALSE(stock.is_suspended(eight_digit_time(20160908), index));
//    //EXPECT_NEAR(stock.trade_data()[i].open_percent(), 2.3, 0.1);
//}
//
//TEST(StockTest, InitTradeDataBlock) {
//    Stock stock("000002", "XXXX", TradeDataType::Day, eight_digit_time(20150101));
//    Stock index("index000001", "XXXX", TradeDataType::Day, eight_digit_time(20150101));
//
//    stock.InitTradeDataBlock(index);
//    int i1 = index.GetIndex(eight_digit_time(20151218));
//    int i2 = index.GetIndex(eight_digit_time(20151221));
//    int i3 = index.GetIndex(eight_digit_time(20160703)); // Sunday
//    int i4 = index.GetIndex(eight_digit_time(20160704));
//    int i5 = index.GetIndex(eight_digit_time(20160705));
//
//    EXPECT_EQ(stock.trade_date_block()[i1]->begin_time, (year_month_day_hour_min_sec)eight_digit_time(20151218));
//    EXPECT_EQ(stock.trade_date_block()[i2], nullptr);
//    EXPECT_EQ(stock.trade_date_block()[i4]->begin_time, (year_month_day_hour_min_sec)eight_digit_time(20160704));
//    EXPECT_EQ(stock.trade_date_block()[i5]->begin_time, (year_month_day_hour_min_sec)eight_digit_time(20160705));
//}
