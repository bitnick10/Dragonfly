#pragma once

#include "Futures.h"
#include "Market.h"

class FuturesMarket: public Market {
public:
    //std::vector<std::unique_ptr<Futures>> futures;
    const std::string DATA_ROOT_PATH = "D:\\Data Center\\bourgogne\\";
    FuturesMarket(DateTime beginDate, DateTime endDate = DateTime("2020-01-01")) {
        AddFutures(beginDate, endDate);
        // InitFuturesMTI();
    }
    void AddFutures(DateTime beginDate, DateTime endDate) {
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

            std::unique_ptr<Futures> s(new Futures(id, name, beginDate, endDate));
            //s.get()->name_ = name;
            if (s->sticks().size() == 0)
                continue;
            charts.push_back(std::move(s));

        } while (sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    Futures* GetFuturesPointerById(const std::string& id) {
        auto ps = std::find_if(charts.begin(), charts.end(), [ = ](const std::unique_ptr<Chart>& s) {
            return s->id() == id;
        });
        return (Futures*)(*ps).get();
    }
};