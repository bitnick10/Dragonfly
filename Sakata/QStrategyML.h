#pragma once

#include "QStrategyBase.h"

struct Switch1 {
    bool on;
    bool value;
};
struct Switch2 {

};
struct QStrategyParameters {
    bool switch_on_type1[6];
    bool bools_type1[6];
    // type 2 with values
    bool switch_on_type2[2];
    bool bools_type2[2];
    double values[2];
    std::string hex_string() {
        return StructToHexString(reinterpret_cast<const unsigned char*>(this), sizeof(QStrategyParameters));
    }
    void operator = (const std::string& hex) {
        HexStringToStruct(hex, reinterpret_cast<unsigned char*>(this), sizeof(QStrategyParameters));
    }
    /*void PrintParameters() {
        for (int i = 0; i < 6; i++) {
            std::cout << switch_on[i] << " ";
        }
        std::cout << ", ";
        std::cout << value1 << " ";
        std::cout << value2 << " ";
        std::cout << value3 << " ";
        std::cout << value4 << " ";
        std::cout << value5 << " ";
        std::cout << value6 << " ";
    }*/
};
class QStrategyML : public QStrategyBase {
public:
    QStrategyML(QStockMarket& market) : QStrategyBase(market) {}
    QStrategyParameters parameters;
    void SetRandomParameters() {

        for (int i = 0; i < 6; i++) {
            bool randomval = rand() & 1;
            parameters.switch_on[i] = randomval;
            // cout << randomval << endl;
        }
        parameters.value1 = rand() & 1;
        parameters.value2 = rand() & 1;
        parameters.value3 = rand() & 1;
        parameters.value4 = rand() & 1;
        parameters.value5 = (double)(rand() % 10);
        parameters.value6 = (double)(rand() % 10);

    }
    bool IsQualified(const Stock& stock, year_month_day_hour_min_sec date) {
        if (stock.trade_data().size() < 10) {
            return false;
        }
        //fmt::print("IsQualified GetIndex {0}\n", date.ToString());
        int i = stock.GetIndex(date);
        // fmt::print("IsQualified GetIndex OVER {0}\n", date.ToString());
        if (i == -1)
            return false;
        if (i < 3)
            return false;
        assert(i != 0);
        assert(stock.trade_data()[i].prev != nullptr);
        assert(stock.trade_data().size() >= 10);

        //std::cout << "IsQualified";
        if (stock.trade_data()[i].prev == nullptr)
            std::cout << "== nullptr";
        if (parameters.switch_on_type1[0]) {
            if (stock.trade_data()[i - 1].is_stick_up() != parameters.bools[0])
                return false;
        }
        if (parameters.switch_on_type1[1]) {
            if (stock.trade_data()[i].is_stick_up() != parameters.bools[1])
                return false;
        }
        if (parameters.switch_on[2]) {
            if (stock.trade_data()[i].open < stock.trade_data()[i - 1].close != parameters.bools[])
                return false;
        }
        if (parameters.switch_on[3]) {
            if (stock.trade_data()[i].close > stock.trade_data()[i - 1].open != parameters.bools[])
                return false;
        }
        if (parameters.switch_on[5]) {
            // 8.0
            if (stock.trade_data()[i].amplitude() < parameters.value6 / 100 != parameters.bools[])
                return false;
        }
        if (parameters.switch_on[4]) {
            // value5 2.0
            if (stock.trade_data()[i - 1].body_amplitude() > parameters.value5 / 100 != parameters.bools[])
                return false;
        }
        if (true
                && stock.indicators().vr()[i] < 100.0
                && stock.indicators().arbr()[i].ar < 150.0
                && stock.indicators().arbr()[i].br < 150.0
           )
            return true;
        return false;
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
           )
            return true;
        else
            return false;
    }
    // close position
    void TradingClose(const QStockMarket & market) {
        if (!account.positions.empty()) {
            // close position conditions
            std::vector<QPosition> copyPositions;
            std::copy(account.positions.begin(), account.positions.end(), std::back_inserter(copyPositions));
            for (auto && p : copyPositions) {
                // const auto& stock = *p.stock;
                // std::cout << "Trading DEBUG 2 \n";
                //std::cout << "id " << p.stock_id << std::endl;
                assert(!p.stock_id().empty());
                int index = p.stock->GetIndex(market.qtime_now);
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
                double close_price = p.stock->trade_data()[index].close;
                account.ClosePosition(p.stock_id(), close_price);
                //i--;
            }
        }
    }
    void Trading(const QStockMarket & market) {
        TradingClose(market);
        for(auto && pstock : market.stocks0s6s) {
            const Stock& stock = *pstock;
            if (IsQualified(stock, market.qtime_now)) {
                int i = stock.GetIndex(market.qtime_now);
                account.OpenPosition(pstock, stock.trade_data()[i].close);
            }
        }
    }
};