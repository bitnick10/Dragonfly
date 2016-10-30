#pragma once

#include "../Dragonfly/QStockMarket.h"

#include "QStrategyBase.h"

/// yang bao yin
class QStrategyYby : public QStrategyBase {
public:
    QStrategyYby(QStockMarket& market) : QStrategyBase(market) {}
    bool IsAlwaysDown(const Stock& stock, int i, int n) {
        for (int j = 0; j < n; j++) {
            if (i - j < 0) {
                return false;
            }
            if (stock.trade_data()[i - j].is_price_up()) {
                return false;
            }
        }
        return true;
    }
    bool static IsStockDataQualified(const Stock& stock) {
        //fmt::print("IsStockDataQualified\n");
        if (stock.trade_data().size() < 10) {
            return false;
        }
        return true;
    }
    bool static IsQualified(const Stock& stock, year_month_day_hour_min_sec date) {
        if (stock.id()[0] == '6' || stock.id()[0] == '0') {
            if (!IsStockDataQualified(stock))
                return false;
            //fmt::print("IsQualified GetIndex {0}\n", date.ToString());
            int i = stock.GetIndex(date);
            // fmt::print("IsQualified GetIndex OVER {0}\n", date.ToString());
            if (i == -1)
                return false;
            if (i == 0)
                return false;
            assert(i != 0);
            assert(stock.trade_data()[i].prev != nullptr);
            assert(stock.trade_data().size() >= 10);

            if (stock.trade_data()[i].prev == nullptr)
                std::cout << "== nullptr";

            if (true
                    && stock.trade_data()[i].prev->is_stick_filled()
                    && stock.trade_data()[i].is_stick_up()
                    && stock.trade_data()[i].open < stock.trade_data()[i].prev->close
                    && stock.trade_data()[i].close > stock.trade_data()[i].prev->open
                    //&& stock.trade_data()[i].close > stock.trade_data()[i - 1].body_middle()

                    && stock.trade_data()[i - 1].body_amplitude() > 2.0 / 100
                    && stock.trade_data()[i].amplitude() < 8.0 / 100

                    //&& stock.trade_data()[i].upper_shadow_amplitude() < 1.7 / 100

                    && stock.trade_data()[i].upper_shadow_amplitude() < stock.trade_data()[i].body_amplitude() * 0.382

                    //&& s->trade_data()[i - 1].open_percent() > -2.0 / 100

                    //&& !IsAlwaysDown(*s, i - 1, 5)

                    //&& s->trade_data()[i].low < s->trade_data()[i].prev->low
                    //&& s->trade_data()[i - 1].volume < s->trade_data()[i - 2].volume * 1.3
                    && stock.indicators().vr()[i] < 100.0
                    && stock.indicators().arbr()[i].ar < 150.0
                    && stock.indicators().arbr()[i].br < 150.0
                    //&& stock.trade_data()[i - 1].close > stock.indicators().boll()[i - 1].lower
                    // && s->trade_data()[i].open_percent() < -1.0 / 100
                    // && s->indicators().kdj()[i].j > 50
                    //&& s->indicators().asi()[i] > s->indicators().asi()[i - 1]
                    // && s->indicators().asi()[i - 1] < s->indicators().asi()[i - 2]
               )
                return true;
            else
                return false;
        } else
            return false;
    }
    void Trading(const QStockMarket & market) {
        //std::cout << "Trading DEBUG";
        if (!account.positions.empty()) {
            // close position conditions
            std::vector<QPosition> copyPositions;
            std::copy(account.positions.begin(), account.positions.end(), std::back_inserter(copyPositions));
            for (auto && p : copyPositions) {
                const auto& stock = *p.stock;
                // std::cout << "Trading DEBUG 2 \n";
                //std::cout << "id " << p.stock_id << std::endl;
                assert(!p.stock_id().empty());
                int index = stock.GetIndex(market.qtime_now);
                if (index == -1)
                    continue;
                if (index == 0)
                    continue;
                // stop loss
                /*if ((stock.trade_data()[i].open - stock.trade_data()[i - 1].close) / stock.trade_data()[i - 1].close < -1.0 / 100) {
                account.ClosePosition(p.stock_id, stock.trade_data()[i].open);
                continue;
                }*/
                /* if ((stock.trade_data()[i].low - stock.trade_data()[i - 1].close) / stock.trade_data()[i - 1].close < -3.0 / 100) {
                account.ClosePosition(p.stock_id, stock.trade_data()[i - 1].close * (1 - 3.0 / 100));
                continue;
                }*/
                // normal close
                //std::cout << "Trading DEBUG 3 ";
                double close_price = market[p.stock_id()].trade_data()[index].close;
                account.ClosePosition(p.stock_id(), close_price);
                //i--;
            }
        }
        // std::cout << "for (auto && s : market.stocks)" << std::endl;
        for (size_t i = 0; i < market.stocks.size(); i++) {
            const Stock* pstock = market.stocks[i].get();
            const Stock& stock = *pstock;
            //assert(s != nullptr);
            // std::cout << stock.id() << std::endl;
            if (IsQualified(stock, market.qtime_now)) {
                int i = stock.GetIndex(market.qtime_now);
                account.OpenPosition(pstock, stock.trade_data()[i].close);
            }
            // std::cout << stock.id() << " O " << std::endl;
            //std::cout << s->id() << std::endl;
            //if (s->id() == "600354") {
            //    std::cout << "oh this " << s->id() << std::endl;
            //    std::cout << market.qtime_now << std::endl;
            //    std::cout << s->indicators().arbr()[i].ar << std::endl;
            //}
            //fmt::print("{0} BUY  {1}\n", market.qtime_now.ToString(), s->trade_data()[i].close);
            //std::cout << market.qtime_now << "BUY " << << std::endl;


        }
    }
};