#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>

#include "Stock.h"
#include "sqlite/sqlite3.h"
#include "time.h"
#include "SQLiteDB.h"

class Market {
public:
    year_month_day_hour_min_sec begin_time;
    std::vector<std::unique_ptr<Stock>> stocks;
    const std::string DATA_ROOT_PATH = "D:\\Data Center\\bourgogne\\";
    double load_time;// in sec
public:
    year_month_day_hour_min_sec last_trade_date() const {
        return this->stocks[0]->trade_data().back().begin_time;
    }
    void AddIndex(eight_digit_time stockDataBeginDate) {
        std::unique_ptr<Stock> sh(new Stock("index000001", "上证指数", TradeDataType::Day, stockDataBeginDate));
        stocks.push_back(std::move(sh));

        std::unique_ptr<Stock> sz(new Stock("index399001", "深证成指", TradeDataType::Day, stockDataBeginDate));
        stocks.push_back(std::move(sz));
    }
    void AddFutures(eight_digit_time stockDataBeginDate) {
        sqlite3 *db;
        sqlite3_stmt *stmt;
        const char *zTail;
        int r = sqlite3_open("D:\\Data Center\\bourgogne\\basics_futures.db", &db);
        if (r) {
            printf("%s", sqlite3_errmsg(db));
            fprintf(stderr, "%s: %s\n", "open", sqlite3_errmsg(db));
            return;
        }
        const char *zSql = "SELECT id,name FROM basics order by id";
        sqlite3_prepare_v2(db, zSql, -1, &stmt, &zTail);
        r = sqlite3_step(stmt);
        do {
            std::string id = (char*)sqlite3_column_text(stmt, 0);
            std::string name = (char*)sqlite3_column_text(stmt, 1);
            //double pe = sqlite3_column_double(stmt, 2);
            //std::cout << " pe " << pe;
            //name = UTF_82ASCII(name);
            std::unique_ptr<Stock> s(new Stock(id, name, 0, TradeDataType::Day, stockDataBeginDate));
            stocks.push_back(std::move(s));
        } while (sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        /* stocks.push_back(std::unique_ptr<Stock>(new Stock("OY", "纽约银连续", TradeDataType::Day, stockDataBeginDate)));
         stocks.push_back(std::unique_ptr<Stock>(new Stock("@GC0Y", "纽约金连续", TradeDataType::Day, stockDataBeginDate)));

         stocks.push_back(std::unique_ptr<Stock>(new Stock("TJAG00", "现货白银", TradeDataType::Day, stockDataBeginDate)));

         stocks.push_back(std::unique_ptr<Stock>(new Stock("AGUSDO", "伦敦银", TradeDataType::Day, stockDataBeginDate)));
         stocks.push_back(std::unique_ptr<Stock>(new Stock("AUUSDO", "伦敦金", TradeDataType::Day, stockDataBeginDate)));
         stocks.push_back(std::unique_ptr<Stock>(new Stock("PTUSDO", "伦敦铂金", TradeDataType::Day, stockDataBeginDate)));

         stocks.push_back(std::unique_ptr<Stock>(new Stock("USDIND", "美元指数", TradeDataType::Day, stockDataBeginDate)));*/
    }
    void AddStocks(eight_digit_time stockDataBeginDate) {
        sqlite3 *db;
        sqlite3_stmt *stmt;
        const char *zTail;
        int r = sqlite3_open("D:\\Data Center\\bourgogne\\basics.db", &db);
        if (r) {
            printf("%s", sqlite3_errmsg(db));
            fprintf(stderr, "%s: %s\n", "open", sqlite3_errmsg(db));
            return;
        }
        const char *zSql = "SELECT code,name,industry,pe FROM basics order by code";
        sqlite3_prepare_v2(db, zSql, -1, &stmt, &zTail);
        r = sqlite3_step(stmt);
        do {
            char* pstr = (char*)sqlite3_column_text(stmt, 0);
            std::string id = pstr;
            std::string name = (char*)sqlite3_column_text(stmt, 1);
            char* pindustry = (char*)sqlite3_column_text(stmt, 2);
            std::string industry;
            if (pindustry != nullptr)
                industry = pindustry;
            double pe = sqlite3_column_double(stmt, 3);
            //std::cout << " pe " << pe;
            //name = UTF_82ASCII(name);
            std::unique_ptr<Stock> s(new Stock(id, name, pe, TradeDataType::Day, stockDataBeginDate));
            s->industry_ = industry;
            stocks.push_back(std::move(s));
        } while (sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    void MyRemove() {
        std::vector<std::unique_ptr<Stock>>::iterator iter = stocks.begin();
        while (iter != stocks.end()) {
            if ((*iter)->id() < "600000") {
                iter = stocks.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    Market() {}
    //Market(year_month_day_hour_min_sec beginDate) {
    //    std::time_t tm = beginDate.epoch();
    //    struct tm *aTime = localtime(&tm);

    //    int day = aTime->tm_mday;
    //    int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    //    int year = aTime->tm_year + 1900; // Year is # years since 1900
    //    eight_digit_time e8time = year * 10000 + month * 100 + day;
    //    Market(e8time);
    //}
    Market(eight_digit_time stockDataBeginDate) {
        std::cout << "Market construct" << std::endl;
        clock_t begin = clock();
        begin_time = stockDataBeginDate;

        AddIndex(stockDataBeginDate);
        AddFutures(stockDataBeginDate);
        AddStocks(stockDataBeginDate);

        std::cout << "Market cons finished" << std::endl;
        clock_t end = clock();
        load_time = double(end - begin) / CLOCKS_PER_SEC;
    }
    std::vector<const Stock*> FindPastDaysLowFallsFromTopInNDays(unsigned int nDays, double downPercent, unsigned int pastn) {
        assert(nDays > pastn);
        std::vector<const Stock*> ret;
        for (auto & s : this->stocks) {
            if (s->trade_data().size() < nDays)
                continue;
            if (s->HighestFallPercent(nDays, pastn) >= downPercent) {
                const Stock* ps = s.get();
                ret.push_back(ps);
            }
        }
        return ret;
    }
    std::string GetStockNameById(const std::string & id) const {
        auto ps = std::find_if(stocks.begin(), stocks.end(), [ = ](const std::unique_ptr<Stock>& s) {
            return s->id() == id;
        });
        return (*ps)->name();
    }
    const Stock& GetStockRefById(const std::string & id) const {
        auto ps = std::find_if(stocks.begin(), stocks.end(), [ = ](const std::unique_ptr<Stock>& s) {
            return s->id() == id;
        });
        return **ps;
    }
    double GetHighestFallPercent(const std::string& id, int n, int pastn) const {
        auto ps = std::find_if(stocks.begin(), stocks.end(), [ = ](const std::unique_ptr<Stock>& s) {
            return s->id() == id;
        });
        return (*ps)->HighestFallPercent(n, pastn);
    }
    virtual const Stock& operator [](const std::string& stock_id) const {
        return GetStockRefById(stock_id);
    }
};