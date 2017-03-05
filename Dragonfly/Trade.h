#pragma once

#include <iostream>
#include <map>
#include <cassert>

#include "Candlestick.h"
#include "MACD.h"
#define FMT_HEADER_ONLY
#include "fmt\format.h"
#include "Chart.h"

enum class TradeDataPeriod {
    Minutes5,
    Minutes15,
    Minutes30,
    Minutes60,
    Day,
    End
};
inline std::string EnumToString(TradeDataPeriod p) {
    switch (p) {
    case TradeDataPeriod::Minutes5:
        return "m5";
    case TradeDataPeriod::Minutes15:
        return "m15";
    case TradeDataPeriod::Minutes30:
        return "m30";
    case TradeDataPeriod::Minutes60:
        return "m60";
    case TradeDataPeriod::Day:
        return "day";
    case TradeDataPeriod::End:
        return "end";
    default:
        return "default";
    }
}

class Trade {
protected:
    std::vector<std::unique_ptr<Chart>> charts_;
public:
    Trade() {
    }
    std::vector<std::unique_ptr<Chart>>& charts() {
        return charts_;
    }
    Chart* chart(int minutes) {
        if(minutes == 5)
            return charts_[(int)TradeDataPeriod::Minutes5].get();
        if (minutes == 15)
            return charts_[(int)TradeDataPeriod::Minutes15].get();
        if (minutes == 30)
            return charts_[(int)TradeDataPeriod::Minutes30].get();
        if (minutes == 60)
            return charts_[(int)TradeDataPeriod::Minutes60].get();
        if (minutes == 1440)
            return charts_[(int)TradeDataPeriod::Day].get();
        assert(false);
        return nullptr;
    }
    Chart* chart(TradeDataPeriod period) {
        return charts_[(int)period].get();
    }
    void UpdateCharts() {
        return;
        if (chart(5) == nullptr) {
            assert(false);
        }

        std::vector<Candlestick> sticks15minutes;
        const std::vector<Candlestick>& sticks5min = this->chart(5)->sticks();
        for (size_t i = 0; i < sticks5min.size(); i += 3) {
            Candlestick newStick1 = Combine(sticks5min[i], sticks5min[i + 1]);
            Candlestick newStick = Combine(newStick1, sticks5min[i + 2]);
            sticks15minutes.push_back(newStick);
        }
        std::unique_ptr<Chart> chart(new Chart(sticks15minutes));
        this->charts_[(int)TradeDataPeriod::Minutes15] = std::move(chart);
        Update30MinutesChart();
        Update60MinutesChart();
    }
    void Update30MinutesChart() {
        if (chart(15) == nullptr) {
            assert(false);
        }
        std::vector<Candlestick> sticks30minutes;
        const std::vector<Candlestick>& sticks15min = this->chart(15)->sticks();
        for (size_t i = 0; i < sticks15min.size(); i++) {
            std::string dayOpen = sticks15min[i].trade_time.ToString().substr(0, 10) + " " + "09:00";
            std::string dayClose = sticks15min[i].trade_time.ToString().substr(0, 10) + " " + "15:00";
            if (i + 1 < sticks15min.size()
                    && dayOpen <= sticks15min[i].trade_time.ToString() && sticks15min[i].trade_time.ToString() <= dayClose
                    && dayOpen <= sticks15min[i + 1].trade_time.ToString() && sticks15min[i + 1].trade_time.ToString() <= dayClose) {
                Candlestick newStick = Combine(sticks15min[i], sticks15min[i + 1]);
                sticks30minutes.push_back(newStick);
                i++;
            } else {
                Candlestick newStick = sticks15min[i];
                sticks30minutes.push_back(newStick);
            }
        }
        std::unique_ptr<Chart> chart(new Chart(sticks30minutes));
        this->charts_[(int)TradeDataPeriod::Minutes30] = std::move(chart);
    }
    void Update60MinutesChart() {
        if (chart(30) == nullptr) {
            assert(false);
        }
        std::vector<Candlestick> sticks60minutes;
        const std::vector<Candlestick>& sticks30min = this->chart(30)->sticks();
        for (size_t i = 0; i < sticks30min.size(); i++) {
            std::string dayOpen = sticks30min[i].trade_time.ToString().substr(0, 10) + " " + "09:00";
            std::string dayClose = sticks30min[i].trade_time.ToString().substr(0, 10) + " " + "15:00";
            if (i + 1 < sticks30min.size()
                    && dayOpen <= sticks30min[i].trade_time.ToString() && sticks30min[i].trade_time.ToString() <= dayClose
                    && dayOpen <= sticks30min[i + 1].trade_time.ToString() && sticks30min[i + 1].trade_time.ToString() <= dayClose) {
                Candlestick newStick = Combine(sticks30min[i], sticks30min[i + 1]);
                sticks60minutes.push_back(newStick);
                i++;
            } else {
                Candlestick newStick = sticks30min[i];
                sticks60minutes.push_back(newStick);
            }
        }
        std::unique_ptr<Chart> chart(new Chart(sticks60minutes));
        this->charts_[(int)TradeDataPeriod::Minutes60] = std::move(chart);
    }
};