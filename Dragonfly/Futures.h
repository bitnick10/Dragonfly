#pragma once

#include "Trade.h"
#include "util.h"
#include "sqlite/sqlite3.h"

#pragma warning(disable: 4244) // warning C4244: conversion from 'double' to 'float', possible loss of data

class Futures : public Trade {
public:
    std::string id_;
    const std::string& id() const {
        return id_;
    }
    Futures() {
    }
    Futures(const std::string& id, DateTime beginDate, DateTime endDate) {
        id_ = id;
        std::string path = "D:\\Data Center\\bourgogne\\k_data_5min\\" + this->id() + ".db";
        LoadData(path, beginDate, endDate);
    }
public:
    static Futures InitFrom5minutesChart(const std::vector<Candlestick>& sticks) {
        Futures futures;
        std::unique_ptr<Chart> chart(new Chart(sticks));
        futures.charts_.resize((int)TradeDataPeriod::End);
        futures.charts_[(int)TradeDataPeriod::Minutes5] = std::move(chart);
        futures.UpdateCharts();
        return futures;
    }
private:
    void LoadData(const std::string& fullfilename, DateTime beginDate, DateTime endDate) {
        if (!FileExists(fullfilename)) {
            return;
        }
        sqlite3 *db;
        sqlite3_stmt *stmt;
        const char *zTail;

        int r = sqlite3_open(fullfilename.c_str(), &db);
        if (r) {
            printf("%s", sqlite3_errmsg(db));
            fprintf(stderr, "%s: %s\n", "open", sqlite3_errmsg(db));
            return;
        }
        // char str[128];
        std::string str = fmt::format("SELECT date,open,close,high,low,volume FROM tick_data where '{0}' <= date AND date <= '{1}' order by date", beginDate.ToString(), endDate.ToString());
        //sprintf(str, "SELECT date,open,close,high,low,volume FROM tick_data where %s <= date AND date <= %s order by date", beginDate.time.c_str(), endDate.time.c_str());
        const char *zSql = str.c_str();
        r = sqlite3_prepare_v2(db, zSql, -1, &stmt, &zTail);
        if (r != SQLITE_OK) {
            fprintf(stderr, "%s:%s %s\n", "sqlite3_prepare_v2", this->id().c_str(), sqlite3_errmsg(db));
            return;
        }
        std::vector<Candlestick> sticks;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Candlestick d;
            std::string strDate = (char*)sqlite3_column_text(stmt, 0);
            d.trade_time = strDate;
            //int column0 = sqlite3_column_int(stmt, 0);
            //eight_digit_time digit_time = eight_digit_time(column0);
            //d.begin_time = digit_time;
            d.open = sqlite3_column_double(stmt, 1);
            d.close = sqlite3_column_double(stmt, 2);
            d.high = sqlite3_column_double(stmt, 3);
            d.low = sqlite3_column_double(stmt, 4);
            d.volume = sqlite3_column_double(stmt, 5);

            sticks.push_back(d);
        }
        std::unique_ptr<Chart> chart(new Chart(sticks));
        this->charts_.resize((int)TradeDataPeriod::End);
        this->charts_[(int)TradeDataPeriod::Minutes5] = std::move(chart);
        this->UpdateCharts();
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};