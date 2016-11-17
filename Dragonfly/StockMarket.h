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
        //std::cout << "StockMarket construct " << beginDate << std::endl;
        AddIndex(beginDate);
        AddStocks(beginDate);
        RemoveBadDataStock("600845"); // source data can not use
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

        //std::cout << "StockMarket construct finished" << std::endl;
    }
    void RemoveBadDataStock(const std::string& stock_id) {
        std::vector<std::unique_ptr<Stock>>::iterator iter = this->stocks.begin();
        while (iter != this->stocks.end()) {
            if ((*iter)->id() == stock_id) {
                iter = this->stocks.erase(iter);
            } else {
                ++iter;
            }
        }
    }
};