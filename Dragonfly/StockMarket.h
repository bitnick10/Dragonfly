#pragma once

#include <string>
#include <fstream>
#include <streambuf>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"

#include "Market.h"

class StockMarket : public Market {
public:
    std::vector<Stock*> stocks0s6s;
    std::vector<Stock*> index;
public:
    StockMarket(eight_digit_time beginDate) {
        std::cout << "StockMarket construct" << std::endl;
        AddIndex(beginDate);
        AddStocks(beginDate);
        for (auto && s : this->stocks) {
            if (s->id()[0] == '0' || s->id()[0] == '6') {
                Stock* sp = s.get();
                stocks0s6s.push_back(sp);
            }
        }
        for (auto && s : this->stocks) {
            if (s->id()[0] == 'i') {
                Stock* sp = s.get();
                index.push_back(sp);
            }
        }
        assert(stocks0s6s.size() > 1000);
        assert(index.size() > 1);

        for (auto && s : this->stocks) {
            s->InitTradeDataBlock(*index[0]);
        }

        std::cout << "StockMarket construct finished" << std::endl;
    }
};