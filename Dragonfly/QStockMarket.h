#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <fstream>

#include "Market.h"
#include "StockMarket.h"

class QStockMarket : public StockMarket {
public:
    int now_i;
    int begin_i;
    int end_i; // included
public:
    // [beginDate,endDate]
    QStockMarket(eight_digit_time beginDate, eight_digit_time endDate) : StockMarket(beginDate - 100) {
        //this->begin_time = beginDate;
        //this->end_time = endDate;
        //this->qtime_now = beginDate;
        std::cout << "QStockMarket Construct " << beginDate << " " << endDate << std::endl;
        this->begin_i = index[0]->GetIndex(index[0]->GetLatestTradingTime(beginDate));
        this->end_i = index[0]->GetIndex(index[0]->GetLatestTradingTime(endDate));
        this->now_i = begin_i;
    }
    void ResetTime() {
        now_i = begin_i;
    }
    bool has_next_day() const {
        return (now_i <= end_i);
    }

    int number_of_trade_day() const {
        return end_i - begin_i + 1;
    }
    void NextDay() {
        now_i++;
    }
    year_month_day_hour_min_sec time_now() const {
        return index[0]->trade_data_block()[now_i]->begin_time;
    }
    year_month_day_hour_min_sec begin_time() const {
        return index[0]->trade_data_block()[begin_i]->begin_time;
    }
    year_month_day_hour_min_sec end_time() const {
        return index[0]->trade_data_block()[end_i]->begin_time;
    }
};

class QStock : public Stock {

};
struct QPosition {
public:
    const Stock* stock;
    enum Type {
        Long,
        Short
    };
    Type position_type = Type::Long;
    year_month_day_hour_min_sec create_time;
    year_month_day_hour_min_sec close_time;
    std::string stock_id() {
        return stock->id();
    };
    double create_price;
    double close_price;
    int number_of_position; // 100(min) 1000 5000
    std::string comments;
};
class QPositionStatistics {
public:
    std::vector<QPosition> history;
public:
    void push_back(const QPosition& val) {
        history.push_back(val);
    }
public:
    size_t number_of_trade() const {
        return history.size();
    }
    double win_rate() const {
        if (number_of_trade() == 0)
            return 0.0;
        assert(number_of_trade() > 0);
        int n = 0;
        for (auto && p : history) {
            if (p.close_price > p.create_price) {
                n++;
            }
        }
        return (double)n / number_of_trade();
    }
    double win_average_earn() const {
        if (number_of_trade() == 0)
            return 0.0;
        assert(number_of_trade() > 0);
        int n = 0;
        double earn = 0.0;
        for (auto && p : history) {
            if (p.close_price > p.create_price) {
                n++;
                earn += (p.close_price - p.create_price) / p.create_price;
            }
        }
        return earn / n;
    }
    double loss_average_earn() const {
        if (number_of_trade() == 0)
            return 0.0;
        assert(number_of_trade() > 0);
        int n = 0;
        double earn = 0.0;
        for (auto && p : history) {
            if (p.close_price < p.create_price) {
                n++;
                earn += (p.close_price - p.create_price) / p.create_price;
            }
        }
        return earn / n;
    }
    double average_revenue_per_trade() {
        if (number_of_trade() == 0)
            return 0.0;
        assert(number_of_trade() > 0);
        int n = 0;
        double earn = 0.0;
        for (auto && p : history) {
            n++;
            earn += (p.close_price - p.create_price) / p.create_price;
        }
        return earn / n;
    }
};
class QAccount {
public:
    double init_money;
    double money_;
    double money() const {
        return money_;
    }
    QPositionStatistics history;
    std::vector<QPosition> positions;
    double commission = 5.0 / 100 / 100;
    double tax = 0.1 / 100;
    std::string report;
    QAccount() {
        this->init_money = 100000;
        this->money_ = 100000;
    }
    double market_cap() const {
        double cap = money();
        // std::cout << "cap1() " << cap << std::endl;
        for (auto iter = positions.begin(); iter != positions.end(); ++iter) {
            //std::cout << "itering" << std::endl;
            //auto time = (*iter).stock->GetLatestTradingTime(market.qtime_now);
            //int i = (*iter).stock->GetIndex((*iter));
            cap += (*iter).number_of_position * (*iter).create_price;
        }
        // std::cout << "cap2() " << cap << std::endl;
        return cap;
    }
    std::string GetRandomString() {
        year_month_day_hour_min_sec now; // use for generate hash string
        std::string str = now.ToString("%Y%m%d %H%M%S");
        std::size_t str_hash = std::hash<std::string> {}(str);
        //std::string folder = fmt::format("D:\\Data Center\\{0}MTI{1}-{2} {3}", this->folder_prefix, mti_name, date.ToString("%Y%m%d"), std::to_string(str_hash));
        return std::to_string(str_hash);
    }
    double annual_revenue(int number_of_trade_day) const {
        if (number_of_trade_day == 0)
            return 0.0;
        double ar = (market_cap() - init_money) / init_money * 243 / number_of_trade_day; // one year trade day = 243
        return ar;
    }
    void GenerateReport(const std::string& beginTime, const std::string& endTime, int number_of_trade_day) {
        // std::string report;
        double temp_money = init_money;
        for (auto && p : history.history) {
            int max_n = MaxNumberToBuy(temp_money, p.create_price);
            temp_money -= p.create_price * p.number_of_position;
            double earn = p.close_price * p.number_of_position;
            temp_money += earn * (1 - commission - tax);
            report += fmt::format("{0} O {1} {2} {3} {4}\n",
                                  p.create_time.ToString(),
                                  p.stock_id(),
                                  p.stock->name(),
                                  p.create_price,
                                  p.number_of_position
                                 );
            report += fmt::format("{0} C {1} {2} {3}", p.close_time.ToString(), p.stock_id(), p.stock->name(), p.close_price);
            report += fmt::format(" earn {0:.2f}%, fee {1} money {2}          {3}\n",
                                  (p.close_price - p.create_price) / p.create_price * 100,
                                  p.close_price * p.number_of_position * (commission + tax),
                                  temp_money,
                                  p.comments
                                 );
        }
        report += "------------------------\n";
        for (auto && p : positions) {
            report += fmt::format("{0} O {1} {2} {3}\n", p.create_time.ToString(), p.stock_id(), p.create_price, p.number_of_position);
        }
        report += "------------------------\n";
        //std::cout << market_cap() << "   " << init_money;
        report += fmt::format("trade days:{0}\n", number_of_trade_day);
        report += fmt::format("trade count:{0}, win rate:{1:.2f}% annual revenue:{2:.2f}%\n", history.number_of_trade(), history.win_rate() * 100, annual_revenue(number_of_trade_day) * 100);
        report += fmt::format("win average earn:{0:.2f}% loss average earn:{1:.2f}\n", history.win_average_earn() * 100, history.loss_average_earn() * 100);
        std::string fullfilename = fmt::format("D:\\Data Center\\StraA {0}-{1} {2}.txt", beginTime, endTime, GetRandomString());
        std::ofstream out(fullfilename);
        out << report;
        out.close();
    }
    int MaxNumberToBuy(double money_to_buy, double price) {
        int ret;
        ret = (int)(money_to_buy / price) / 100 * 100;
        return ret;
    }
};
