#pragma once

#include <iostream>

#include "sqlite/sqlite3.h"
#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"
class SQLiteDB {
public:
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *zSql;
    const char *pzTail;

    SQLiteDB(std::string fullfilename) {
        int r = sqlite3_open(fullfilename.c_str(), &db);
        if (r) {
            printf("%s", sqlite3_errmsg(db));
            fprintf(stderr, "%s: %s\n", "open", sqlite3_errmsg(db));
            return;
        }
    }
    void DrapTableIfExists(const std::string& table_name) {
        std::string query = fmt::format("trop talbe if exists {0}", table_name);
        int r = sqlite3_exec(db, query.c_str(), 0, 0, 0);
    }
    void ClearTable(const std::string& table_name) {
        std::string query = fmt::format("DELETE FROM {0}", table_name);
        int r = sqlite3_exec(db, query.c_str(), 0, 0, 0);
    }
    void CreateTable(std::string query) {
        const char *zSql = query.c_str();
        int r = sqlite3_prepare(db, zSql, -1, &stmt, &pzTail);
        if (r != SQLITE_OK) {
            fprintf(stderr, "%s: %s\n", "sqlite3_prepare_v2", sqlite3_errmsg(db));
            return;
        }
        r = sqlite3_step(stmt);
    }
    void TransactionBegin() {
        sqlite3_exec(db, "begin;", 0, 0, 0);
    }
    void TransactionCommit() {
        sqlite3_exec(db, "commit;", 0, 0, 0);
    }
    void SynchronousOff() {
        sqlite3_exec(db, "PRAGMA synchronous = OFF; ", 0, 0, 0);
    }
    void Insert(std::string query) {
        const char *zSql = query.c_str();
        int r = sqlite3_prepare(db, zSql, -1, &stmt, &pzTail);
        r = sqlite3_step(stmt);
        if (r != SQLITE_DONE) {
            printf("%s", sqlite3_errmsg(db));
        }
    }
    void PrepareInsert(const std::string& query) {
        const char *zSql = query.c_str();
        int r = sqlite3_prepare(db, zSql, -1, &stmt, &pzTail);
    }
    /*template<typename T1, typename T2, typename T3, typename T4>
    void InsertBindAndStep(T1 a, T2 b, T3 c, T4 d) {
        sqlite3_reset(stmt);
        Bind(1, a);
        Bind(2, b);
        Bind(3, c);
        Bind(4, d);
        sqlite3_step(stmt);
    }*/
    template<typename... Args>
    void InsertBindAndStep(Args... args) {
        sqlite3_reset(stmt);
        BindALot(0, args...);
        sqlite3_step(stmt);
    }
    template<typename T>
    void BindALot(int i, T value) {
        i++;
        Bind(i, value);
    }
    template<typename T, typename... R>
    void BindALot(int i, T first, R... r) {
        i++;
        Bind(i, first);
        BindALot(i, r...);
    }
    void Bind(int i, const std::wstring& value) {
        sqlite3_bind_text16(stmt, i, value.c_str(), -1, SQLITE_STATIC);
    }
    void Bind(int i, const std::string& value) {
        sqlite3_bind_text(stmt, i, value.c_str(), -1, SQLITE_STATIC);
    }
    void Bind(int i, int value) {
        sqlite3_bind_int(stmt, i, value);
    }
    void Bind(int i, double value) {
        sqlite3_bind_double(stmt, i, value);
    }
    // void Bind(int i, const char* value) {
    //    sqlite3_bind_text16(stmt, i, value, -1, SQLITE_STATIC);
    //sqlite3_bind_text(stmt, i, value, -1, SQLITE_STATIC);
    // }

    ~SQLiteDB() {
        int r = 0;
        if(stmt != nullptr)
            r = sqlite3_finalize(stmt);
        if (r != SQLITE_OK) {
            std::cout << "sqlite3_finalize NO OK" << std::endl;
            std::cout << r << std::endl;
        }

        r = sqlite3_close(db);
        if (r != SQLITE_OK) {
            std::cout << "sqlite3_close NOT return SQLITE_OK" << std::endl;
            std::cout << r << std::endl;
            // force close
            while (r == SQLITE_BUSY) {
                // set rc to something that will exit the while loop
                std::cout << "force close" << std::endl;
                r = SQLITE_OK;
                sqlite3_stmt * stmt = sqlite3_next_stmt(db, NULL);

                if (stmt != NULL) {
                    r = sqlite3_finalize(stmt);
                    if (r == SQLITE_OK) {
                        r = sqlite3_close(db);
                    }
                }
            }
        }

    }
};