#pragma once

#include <string>
#include <fstream>
#include <streambuf>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"

#include "Market.h"
class RealtimeMarket : public StockMarket {
public:
    RealtimeMarket(eight_digit_time stockDataBeginDate) : StockMarket(stockDataBeginDate) {
    }
    void Update(eight_digit_time todayDate) {
        TradeData faked_data;
        faked_data.begin_time = todayDate;
        const_cast<std::vector<TradeData>&>(this->index[0]->trade_data()).push_back(faked_data);

        std::cout << "RealtimeMarket Update\n";
        std::string path = DATA_ROOT_PATH + "hdata_raw\\realtime_stock.csv";
        std::ifstream t(path.c_str());
        std::string content((std::istreambuf_iterator<char>(t)),
                            std::istreambuf_iterator<char>());
        //std::cout << content;
        boost::replace_all(content, "\t\t", "\t");

        double volume_proportion;
        year_month_day_hour_min_sec now;
        year_month_day_hour_min_sec today930 = todayDate;
        today930.AddHour(9);
        today930.AddMinutes(30);
        int past_time = now.epoch() - today930.epoch();
        if (past_time < 0) {
            volume_proportion = -1;
            std::cout << "no realtime date,please download first" << std::endl;
            return;
        } else if (past_time < 2 * 60 * 60) {
            volume_proportion = (double)past_time / (4 * 60 * 60);
        } else if (past_time < 3.5 * 60 * 60) {
            volume_proportion = 0.5;
        } else if (past_time < 5.5 * 60 * 60) {
            volume_proportion = (double)(past_time - (1.5 * 60 * 60)) / (4 * 60 * 60);
        } else {
            volume_proportion = 1;
        }
        year_month_day_hour_min_sec timeNow;
        std::cout << timeNow.ToString("%X") << " volume proporttion " << volume_proportion << "   "  << std::endl;

        std::vector<std::string> strs;

        boost::split(strs, content, boost::is_any_of("\n"));
        assert(strs.size() > 1500);
        std::vector<std::tuple<std::string, TradeData>> realtimedata;
        for (auto& s : strs) {
            std::vector<std::string> realtime_trade_data;
            boost::split(realtime_trade_data, s, boost::is_any_of("\t"));

            if (realtime_trade_data.size() >= 5) {
                std::string id = realtime_trade_data[0];
                id = id.substr(2, id.size() - 1);
                //id.erase(std::remove(id.begin(), id.end(), 'SZ'), id.end());
                //id.erase(std::remove(id.begin(), id.end(), 'SH'), id.end());
                TradeData d;
                d.begin_time = todayDate;
                d.close = atof(realtime_trade_data[1].c_str());
                d.volume = atof(realtime_trade_data[2].c_str());
                d.volume /= volume_proportion;
                d.open = atof(realtime_trade_data[3].c_str());
                d.high = atof(realtime_trade_data[4].c_str());
                d.low = atof(realtime_trade_data[5].c_str());

                realtimedata.push_back(std::make_tuple(id, d));

                //std::cout << id << std::endl;
            }
        }
        for (size_t i = 0; i < this->stocks.size(); i++) {
            std::unique_ptr<Stock>& s = this->stocks[i];

            if (s->trade_data().size() < 10)
                continue;
            if (s->trade_data().back().begin_time != todayDate) {
                auto it = std::find_if(realtimedata.begin(), realtimedata.end(), [&](const std::tuple<std::string, TradeData> & o) {
                    if (s->id() == std::get<0>(o))
                        return true;
                    else {
                        return false;
                    }
                });
                if (it != realtimedata.end()) {
                    //std::cout << "Update stock " << std::get<0>(*it) << " ; ";
                    const_cast<std::vector<TradeData>&>(this->stocks[i]->trade_data()).push_back(std::get<1>(*it));
                    s->UpdateDataLink();
                    s->UpdateIndicators();
                    s->InitTradeDataBlock(*index[0]);
                }
            }

        }
        std::cout << "RealtimeMarket Update finished" << std::endl;
        //std::replace(content.begin(), content.end(), "\t\t", "\t");
    }
};