#include <iostream>
#include <vector>

#include "position.h"

namespace quant {
class Account {
public:
    std::vector<Position> closed_positions;
    std::vector<Position> positions;
    void OpenPosition(Position p) {
        positions.push_back(p);
    }
    void ClosePosition(int i, double closePrice, DateTime closeTime) {
        positions[i].close_price = closePrice;
        positions[i].close_time = closeTime;
        closed_positions.push_back(positions[i]);
        auto iter = positions.begin() + i;
        positions.erase(iter);
    }
    double win_rate() const {
        if (closed_positions.size() == 0)
            return 0.0;
        assert(closed_positions.size() > 0);
        double n = 0;
        for (auto && p : closed_positions) {
            if (p.profit_percent() > 0) {
                n += 1;
            }
        }
        return n / closed_positions.size();
    }
    double average_revenue_per_trade() const {
        if (closed_positions.size() == 0)
            return 0.0;
        //assert(number_of_trade() > 0);
        int n = 0;
        double earn = 0.0;
        for (auto && p : closed_positions) {
            n++;
            earn += p.profit_percent();
        }
        assert(n != 0);
        return earn / n;
    }
    void PrintTradeRecords() const {
        for (auto& pos : closed_positions) {
            fmt::print("{0} {1} {2:.2f} {3}\n", pos.create_time.ToString(), pos.type == Position::Type::Long ? "L" : "S", pos.profit_percent() * 100, pos.close_time.ToString());
            //std::cout<<pos.create_time.ToString()<<""
        }
        fmt::print("wr:{0:.2f}%\n", win_rate() * 100);
    }
};
}