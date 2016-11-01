#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <numeric>

#include "../Dragonfly/Market.h"
#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"
#include "../Dragonfly/RealtimeMarket.h"
#include "../Dragonfly/RealtimeFuturesMarket.h"

#include "MTIHelper.h"
#include "MTIExplorer.h"

#pragma warning(disable: 4244)
//void foo() {
//    Stock s("000001", "NAME", TradeDataType::Day, eight_digit_time(20160101));
//    //std::cout << s.kdj().back().j;
//}
void GenerateRealtimeReport(eight_digit_time date, const Market& pool) {
    RealtimeMarket rm(eight_digit_time(20150701));
    rm.Update(date);
    MTIHelper mtihelper;
    mtihelper.folder_prefix = "Realtime Stock";
    mtihelper.using_multithread = true;
    mtihelper.is_raeltime = true;

    auto& mti3s = mtihelper.GetStockMTIList(rm, date, "MTI3");
    auto& mti2s = mtihelper.GetStockMTIList(rm, date, "MTI2");
    std::cout << "Stock mti size = " << mti2s.size() << std::endl;
    //std::cout << mti2s[0].mti->i
    mtihelper.GenerateReport(mti3s, "MTI3", pool, date);
    mtihelper.GenerateReport(mti2s, "MTI2", pool, date);

    // mtihelper.GenerateStockReport(rm, pool, date);
}
void GenerateRealtimeFuturesReport(eight_digit_time date, const Market& pool) {
    RealtimeFuturesMarket rm(eight_digit_time(20150701));
    rm.Update(date);
    MTIHelper mtihelper;
    mtihelper.folder_prefix = "Realtime Futures";
    mtihelper.using_multithread = true;
    mtihelper.is_raeltime = true;

    auto& mti3s = mtihelper.GetFuturesMTIList(rm, date, "MTI3");
    auto& mti2s = mtihelper.GetFuturesMTIList(rm, date, "MTI2");
    assert(mti3s.size() >= 7);
    mtihelper.GenerateReport(mti3s, "MTI3", pool, date);
    mtihelper.GenerateReport(mti2s, "MTI2", pool, date);
}

void main() {
    //eight_digit_time date(20161024);

    StockMarket pool(eight_digit_time(19950101));
    //StockMarket pool(eight_digit_time(20150101));
    Market& m = pool;

    auto begin = clock();
    MTIExplorer explorer;
    eight_digit_time begin_date = 20151201;
    eight_digit_time end_date = 20160101;
    explorer.folder_prefix = "MTI " + std::to_string(begin_date.value()) + "-" + std::to_string(end_date.value());
    auto& mti3s = MTIExplorerHelper::GetMTIList(pool, begin_date, end_date, "MTI3");
    auto& mti2s = MTIExplorerHelper::GetMTIList(pool, begin_date, end_date, "MTI2");
    explorer.GenerateReport(mti3s, "MTI3", pool);
    explorer.GenerateReport(mti2s, "MTI2", pool);
    auto end = clock();
    auto spend = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "spend " << spend << "s" << std::endl;
    return;

    MTIHelper mtihelper;
    mtihelper.using_multithread = true;

    //while (true) {
    //    std::cout << date << "\n";
    //    std::cout << "1:Realtime Stock Report\n2:Futures Report\n3:Stock Report\n4:Realtime Futures Report\n5:A Lot Stock Report" << std::endl;
    //    int choice;
    //    std::cin >> choice;
    //    switch (choice) {
    //    case 1: {
    //        auto begin = clock();
    //        GenerateRealtimeReport(date, pool);
    //        auto end = clock();
    //        auto spend = double(end - begin) / CLOCKS_PER_SEC;
    //        std::cout << "spend " << spend << "s" << std::endl;
    //        break;
    //    }
    //    case 2: {
    //        Market rfm(eight_digit_time(20150801));
    //        mtihelper.GenerateFuturesReport(rfm, pool, date);
    //        break;
    //    }
    //    case 3: {
    //        auto begin = clock();
    //        mtihelper.GenerateStockReport(m, pool, date);
    //        auto end = clock();
    //        auto spend = double(end - begin) / CLOCKS_PER_SEC;
    //        std::cout << "spend " << spend << "s" << std::endl;
    //        break;
    //    }
    //    case 4: {
    //        auto begin = clock();
    //        GenerateRealtimeFuturesReport(date, pool);
    //        auto end = clock();
    //        auto spend = double(end - begin) / CLOCKS_PER_SEC;
    //        std::cout << "spend " << spend << "s" << std::endl;
    //        break;
    //    }
    //    case 5: {
    //        auto begin = clock();

    //        //this->folder_prefix = "Stock";
    //        //auto& mti2s = GetStockMTIList(m, date, "MTI2");
    //        //auto& mti3s = GetStockMTIList(m, date, "MTI3");
    //        //std::cout << "Generate Stock Report Stock MTI size = " << mti2s.size() << std::endl;
    //        //GenerateReport(mti3s, "Stock MTI3", pool, date);
    //        //GenerateReport(mti2s, "Stock MTI2", pool, date);

    //        MTIStatistics stat;
    //        stat.folder_prefix = "Statistics";
    //        auto& mti3s = stat.GetStockMTIList(pool, 20160803, 20161022, "MTI3");
    //        auto& mti2s = stat.GetStockMTIList(pool, 20160803, 20161022, "MTI2");
    //        stat.GenerateReport(mti3s, "Stock STAT MTI3", pool, date);
    //        stat.GenerateReport(mti2s, "Stock STAT MTI2", pool, date);
    //        auto end = clock();
    //        auto spend = double(end - begin) / CLOCKS_PER_SEC;
    //        std::cout << "spend " << spend << "s" << std::endl;
    //        break;
    //    }
    //    default:
    //        break;
    //    }
    //}

    //mtihelper.GenerateStockReport(m, pool, date);
    //mtihelper.GenerateFuturesReport(m, pool, date);
    //GenerateRealtimeReport(date, pool);

    //std::cout << "OY sample " << result.size() << std::endl;
    //fmt::print("load market {0}s ({1} min)\n", m.load_time, m.load_time / 60);
    //std::cout << "total stock number: " << m.stocks.size() << std::endl;
    //std::cout << "last trade date: " << m.last_trade_date() << std::endl;
    //std::cout << "MTIStatistics: " << stat.fit_time << "s" << " (" << stat.fit_time / 60 << " min) " << std::endl;

    return;
}