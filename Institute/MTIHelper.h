#pragma once
#pragma once
#pragma once

#include <iomanip>
#include <functional>
#include <iostream>
#include <experimental/filesystem>
#include <boost/filesystem.hpp>
namespace fs = std::experimental::filesystem;

#include "../Dragonfly/Market.h"
#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"
#include "../Dragonfly/MTI.h"
#include "../Dragonfly/FMTI.h"
#include "../Dragonfly/ThreadPool/ThreadPool.h"
#include "../Dragonfly/encode.h"

struct OneMTI {
    const IMTI* mti = nullptr;
    const Stock* father = nullptr;
    year_month_day_hour_min_sec date;
};

struct MTIInfo {
    OneMTI one_mti;
    std::string mti_name; //MTI2 MTI3
    std::vector<StockAndIndex> fits;
    MTIInfo() {
    }
    ~MTIInfo() {}
    MTIInfo(const MTIInfo&& other) : fits(other.fits) {
        one_mti = other.one_mti;
    }
    MTIInfo(OneMTI one_mti, const Market& pool, std::string mti_name) {
        this->one_mti = one_mti;
        this->mti_name = mti_name;
        //.. if (mti_name == "") {
        this->fits = std::move(this->one_mti.mti->FindFit(pool));
        RemoveFitsSelf();
        // ..} else {
        //    this->fits = std::move(this->one_mti.mti->FindFit2(pool));
        //  }
    }
    void RemoveFitsSelf() {
        auto iter = std::remove_if(fits.begin(), fits.end(), [&](StockAndIndex si) {
            //std::cout << si.stock->id() << std::endl;
            //std::cout << one_mti.father->id() << std::endl;
            //std::cout << "si.i = " << si.i << std::endl;
            //std::cout << "date = " << one_mti.date << std::endl;
            //std::cout << "date i = " << one_mti.father->GetIndex(one_mti.date) << std::endl;
            //std::cout << si.stock->trade_data()[si.i].begin_time << std::endl;
            if (si.stock->id() == one_mti.father->id() && si.stock->trade_data()[si.i].begin_time == one_mti.date) {
                //std::cout << si.stock->id() << std::endl;
                // std::cout << one_mti.father->id() << std::endl;
                //std::cout << "OOOOOOOOOOOOOO" << std::endl;
                return true;
            } else
                return false;
        });
        fits.erase(iter, fits.end());
    }
    double up_average_high() {
        std::vector<double> high_percent;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_up()) {
                high_percent.push_back(f.stock->trade_data()[f.i + 1].high_percent());
            }
        }
        if (high_percent.size() == 0) {
            return 0.0;
        }
        return std::accumulate(high_percent.begin(), high_percent.end(), 0.0) / high_percent.size();
    }
    double up_average_low() {
        std::vector<double> high_percent;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_up()) {
                high_percent.push_back(f.stock->trade_data()[f.i + 1].low_percent());
            }
        }
        if (high_percent.size() == 0) {
            return 0.0;
        }
        return std::accumulate(high_percent.begin(), high_percent.end(), 0.0) / high_percent.size();
    }
    double down_average_high() {
        std::vector<double> high_percent;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_down()) {
                high_percent.push_back(f.stock->trade_data()[f.i + 1].high_percent());
            }
        }
        if (high_percent.size() == 0) {
            return 0.0;
        }
        return std::accumulate(high_percent.begin(), high_percent.end(), 0.0) / high_percent.size();
    }
    double down_average_low() {
        std::vector<double> high_percent;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_down()) {
                high_percent.push_back(f.stock->trade_data()[f.i + 1].low_percent());
            }
        }
        if (high_percent.size() == 0) {
            return 0.0;
        }
        return std::accumulate(high_percent.begin(), high_percent.end(), 0.0) / high_percent.size();
    }
    double average_up() const {
        std::vector<double> up_percent;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_up()) {
                up_percent.push_back(f.stock->trade_data()[f.i + 1].percent_change());
            }
        }
        if (up_percent.size() == 0) {
            return 0.0;
        }
        return std::accumulate(up_percent.begin(), up_percent.end(), 0.0) / up_percent.size();
    }
    double average_down() const {
        std::vector<double> down_percent;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_down()) {
                //std::cout << sf.stock->trade_data()[sf.i + 1].percent_change() << std::endl;
                down_percent.push_back(f.stock->trade_data()[f.i + 1].percent_change());
            }
        }
        if (down_percent.size() == 0) {
            return 0.0;
        }
        auto ret = std::accumulate(down_percent.begin(), down_percent.end(), 0.0) / down_percent.size();
        // std::cout << ret << "           ..." << std::endl;
        return ret;
    }
    double up_probability() const {
        int n = 0;
        for (auto && f : fits) {
            if (f.stock->trade_data()[f.i + 1].is_price_up()) {
                n++;
            }
        }
        return (double)n / fits.size();
    }
};

class MTIHelper {
public:
    bool using_multithread = true;
    std::string folder_prefix = "";
    bool is_raeltime = false;
public:
    //std::vector<std::unique_ptr<MTIInfo>> results;
    void GenerateReport(const std::vector<OneMTI>& mti_list, const std::string& mti_name, const Market& pool, eight_digit_time date) {
        std::vector<std::unique_ptr<MTIInfo>> results;
        if (using_multithread) {
            ThreadPool thread_pool(16);
            std::vector<std::future<std::unique_ptr<MTIInfo>>> future_results;
            for (auto && one : mti_list) {
                future_results.emplace_back(
                thread_pool.enqueue([&]()-> std::unique_ptr<MTIInfo> {
                    fmt::print("Caculating MTI{0}Info {1}\n", mti_name, one.father->id());
                    std::unique_ptr<MTIInfo> pinfo(new MTIInfo(one, pool, mti_name));
                    return std::move(pinfo);
                })
                );
            }
            for (auto && r : future_results) {
                //std::cout << r.get()->father->id() << " finished caculate!\n";
                results.push_back(r.get());
            }
        } else {
            for (auto && one : mti_list) {
                //std::cout << one.father->id() << "\n";
                std::unique_ptr<MTIInfo> pinfo(new MTIInfo(one, pool, mti_name));
                results.push_back(std::move(pinfo));
            }
        }
        SaveToDb(results, date, mti_name);
    }
    void GenerateStockReport(const Market & m, const Market & pool, eight_digit_time date) {
        this->folder_prefix = "Stock";
        auto& mti2s = GetStockMTIList(m, date, "MTI2");
        auto& mti3s = GetStockMTIList(m, date, "MTI3");
        std::cout << "Generate Stock Report Stock MTI size = " << mti2s.size() << std::endl;
        GenerateReport(mti3s, "Stock MTI3", pool, date);
        GenerateReport(mti2s, "Stock MTI2", pool, date);
    }
    void GenerateFuturesReport(const Market & m, const Market & pool, eight_digit_time date) {
        this->folder_prefix = "Futures";
        auto& mti2s = GetFuturesMTIList(m, date, "MTI2");
        auto& mti3s = GetFuturesMTIList(m, date, "MTI3");
        GenerateReport(mti3s, "Futures MTI3", pool, date);
        GenerateReport(mti2s, "Futures MTI2", pool, date);
    }
    static std::vector<OneMTI> GetStockMTIList(const Market & m, eight_digit_time date, std::string mti_name) {
        std::vector<OneMTI> ret;
        for (auto && s : m.stocks) {
            if (s->trade_data().size() < 30) {
                //std::cout << s->id() << " trade_data().size() < 30\n";
                continue;
            }
            if (s->id()[0] == 'i' || s->id()[0] == '0' || s->id()[0] == '6') {
                int i = s->GetIndex(date);
                if (i == -1) {
                    //std::cout << s->id() << date << " date data not found\n";
                    continue;
                }
                // condition

                if (s->name()[0] == 'S' && s->name().find("ST") == std::string::npos) {
                    if (4.95 / 100 < s->trade_data()[i].percent_change() && s->trade_data()[i].percent_change() < 5.05 / 100)
                        continue;
                }
                if (s->name().find("ST") != std::string::npos || s->name().find("*ST") != std::string::npos) {
                    if (4.95 / 100 < s->trade_data()[i].percent_change() && s->trade_data()[i].percent_change() < 5.05 / 100)
                        continue;
                }
                if (true/*s->trade_data()[i].percent_change() < -2.0 / 100*/ /*|| s->trade_data()[i].percent_change() > 9.9 / 100*/) {
                    OneMTI onemti;
                    if (mti_name == "MTI3")
                        onemti.mti = &(s->indicators().mti3()[i]);
                    else {
                        onemti.mti = &(s->indicators().mti2()[i]);
                    }
                    onemti.father = s.get();
                    onemti.date = date;
                    ret.push_back(onemti);
                }
            } else {
                continue;
            }
        }
        return ret;
    }
    std::vector<OneMTI> GetFuturesMTIList(const Market & m, eight_digit_time date, std::string mti_name) {
        std::vector<OneMTI> ret;
        for (auto && s : m.stocks) {
            if (s->trade_data().size() < 30) {
                //std::cout << "s->trade_data().size() < 30";
                continue;
            }
            if (s->id()[0] == 'i') {
                continue;
            }
            if ('0' <= s->id()[0] && s->id()[0] <= '9') {
                continue;
            } else {
                int i = s->GetIndex(date);
                if (i == -1) {
                    std::cout << s->id() << " GetFuturesMTIList data not found";
                    continue;
                }
                OneMTI onemti;
                if (mti_name == "MTI3")
                    onemti.mti = &(s->indicators().fmti3()[i]);
                else {
                    onemti.mti = &(s->indicators().fmti2()[i]);
                }
                onemti.father = s.get();
                onemti.date = date;
                ret.push_back(onemti);
            }
        }
        return ret;
    }

    double MyRound4(double value) {
        return round(value * 10000.0) / 10000.0;
    }
    double MyRound2(double value) {
        return round(value * 100.0) / 100.0;
    }
    std::string GetRandomFolderPath(year_month_day_hour_min_sec date, const std::string& mti_name) {
        year_month_day_hour_min_sec now; // use for generate hash string
        std::string str = now.ToString("%Y%m%d %H%M%S");
        std::size_t str_hash = std::hash<std::string> {}(str);
        std::string folder = fmt::format("D:\\Data Center\\{0}MTI{1}-{2} {3}", this->folder_prefix, mti_name, date.ToString("%Y%m%d"), std::to_string(str_hash));
        return folder;
    }
    void SaveToDb(const std::vector<std::unique_ptr<MTIInfo>>& results, year_month_day_hour_min_sec date, const std::string& mti_name) {
        std::string folder_path;
        if (this->is_raeltime) {
            // realtime data create folder WITHOUT hash
            folder_path = fmt::format("D:\\Data Center\\{0} {1}", this->folder_prefix, mti_name);
        } else {
            folder_path = GetRandomFolderPath(date, mti_name);
        }
        /* if (this->is_raeltime) {
        if (fs::exists(folder_path))
        fs::remove_all(folder_path);
        }*/

        if (!fs::exists(folder_path))
            fs::create_directory(folder_path);

        SQLiteDB db(folder_path + "\\total.db");


        db.CreateTable("CREATE TABLE IF NOT EXISTS stocks(id TEXT,change REAL,samples INTEGER,up_prob REAL,avg_up_percent REAL,avg_down_percent REAL,E REAL,up_avg_high REAL, up_avg_low REAL, down_avg_high REAL, down_avg_low REAL,real_up REAL,comments TEXT);");
        db.ClearTable("stocks");
        db.TransactionBegin();
        //                                                                                                                                               1,2,3,4,5,6,7,8,9,0
        std::string query = "INSERT INTO stocks(id,change,samples,up_prob,avg_up_percent,avg_down_percent,E,up_avg_high, up_avg_low, down_avg_high, down_avg_low,real_up) VALUES(?,?,?,?,?,?,?,?,?,?,?,?);";
        db.PrepareInsert(query);
        for (auto && info : results) {
            int date_index = info->one_mti.father->GetIndex(date);
            if (date_index == -1) {
                std::cout << "-1";
                continue;
            }
            std::string ida = info->one_mti.father->id() + " " + info->one_mti.father->name() + " " + info->one_mti.date.ToString();
            double change = info->one_mti.father->trade_data()[date_index].percent_change();

            std::wstring id(ida.begin(), ida.end());
            //..if ('0' <= ida[0] && ida[0] <= '9') {
            id = Utf82Unicode(ida);
            //} else {
            //    id = Acsi2WideByte(ida);
            //}

            //id = ASCII2UTF_8(id);
            int samples = info->fits.size();
            if (samples == 0) {
                continue;
            }
            double up_prob = info->up_probability();
            double avg_up_percent = info->average_up();
            double avg_down_percent = info->average_down();
            double E = info->average_up() * info->up_probability() + info->average_down() * (1 - info->up_probability());
            double up_avg_high = info->up_average_high();
            double up_avg_low = info->up_average_low();
            double down_avg_high = info->down_average_high();
            double down_avg_low = info->down_average_low();

            double J = info->one_mti.father->indicators().kdj()[date_index].j;
            double deltaJ = info->one_mti.father->indicators().kdj()[date_index].j - info->one_mti.father->indicators().kdj()[date_index - 1].j;
            double vr = info->one_mti.father->indicators().vr()[date_index];

            double real_up = 0.0;
            if (date != info->one_mti.father->trade_data().back().begin_time) {
                real_up = info->one_mti.father->trade_data()[date_index + 1].percent_change();
            }

            up_prob = MyRound4(up_prob);
            avg_up_percent = MyRound4(avg_up_percent);
            avg_down_percent = MyRound4(avg_down_percent);
            E = MyRound4(E);
            up_avg_high = (MyRound4(up_avg_high)) * 100;
            up_avg_low = (MyRound4(up_avg_low)) * 100;
            down_avg_high = (MyRound4(down_avg_high)) * 100;
            down_avg_low = (MyRound4(down_avg_low)) * 100;
            //std::cout<<"indest "
            db.InsertBindAndStep(id, MyRound4(change) * 100, samples, up_prob * 100, avg_up_percent * 100, avg_down_percent * 100, E * 100, up_avg_high, up_avg_low, down_avg_high, down_avg_low, MyRound4(real_up) * 100);
            // auto query = fmt::format("INSERT INTO stocks(id,samples,up_prob,avg_up_percent,avg_down_percent,E) VALUES('{0}',{1},{2},{3},{4},{5});",
            //                          id, samples, up_prob * 100, avg_up_percent * 100, avg_down_percent * 100, E * 100);
            //std::cout << query << std::endl;
            //db.Insert(query);
        }
        db.TransactionCommit();
        SaveDetails(folder_path, results, date, mti_name);
    }
    void SaveDetails(const std::string& folder, const std::vector<std::unique_ptr<MTIInfo>>& results, year_month_day_hour_min_sec date, const std::string& mti_name) {
        SQLiteDB db(fmt::format("{0}\\detail.db", folder));
        //if (this->is_raeltime) {
        //db.DrapTableIfExists("data");
        //}
        db.CreateTable("CREATE TABLE IF NOT EXISTS data(id TEXT,s_id TEXT,date TEXT,change_prev REAL,change REAL,open REAL,high REAL,low REAL);");
        db.ClearTable("data");
        db.SynchronousOff();
        db.TransactionBegin();
        std::string query = "INSERT INTO data(id,s_id,date,change_prev,change,open,high,low) VALUES(?,?,?,?,?,?,?,?);";
        db.PrepareInsert(query);
        for (auto && info : results) {
            if (/*ss->WillSaveDetail()*/true) {
                std::cout << "insert detail " << info->one_mti.father->id() << "\n";
                for (auto && f : info->fits) {
                    std::string similar_id = f.stock->id();
                    std::string date = f.stock->GetDate(f.i + 1).ToString();
                    double change_prev = f.stock->trade_data()[f.i].percent_change();
                    double change = f.stock->trade_data()[f.i + 1].percent_change();

                    double open = (f.stock->trade_data()[f.i + 1].open - f.stock->trade_data()[f.i].close) / f.stock->trade_data()[f.i].close;
                    double low = (f.stock->trade_data()[f.i + 1].low - f.stock->trade_data()[f.i].close) / f.stock->trade_data()[f.i].close;
                    double high = (f.stock->trade_data()[f.i + 1].high - f.stock->trade_data()[f.i].close) / f.stock->trade_data()[f.i].close;
                    // auto query = fmt::format("INSERT INTO data(id, s_id, date, change) VALUES('{0}','{1}','{2}',{3});",
                    //                          ss.stock.id(), id, date, change);
                    db.InsertBindAndStep(Acsi2WideByte(info->one_mti.father->id() + info->one_mti.date.ToString()) , Acsi2WideByte(similar_id), date, MyRound4(change_prev) * 100, MyRound4(change) * 100, MyRound4(open) * 100, MyRound4(high) * 100, MyRound4(low) * 100);
                }

                //ss.SaveToDb(folder);
            }
        }
        db.TransactionCommit();
    }
};
//struct MTIStatistics {
//public:
//    const Market& m;
//    year_month_day_hour_min_sec date;
//    std::vector<std::unique_ptr<FitInfo>> stock_statistics;
//    double fit_time; // in sec
//    int fit_n;
//    MTIStatistics(const Market& m, year_month_day_hour_min_sec date, int fit_n) : m(m) {
//        auto begin = clock();
//        this->date = date;
//        this->fit_n = fit_n;
//        //FindFit_Multithread();
//        auto end = clock();
//        fit_time = double(end - begin) / CLOCKS_PER_SEC;
//    }
//    void FindFit_Multithread() {
//        FindFit_Multithread([](const Stock & stock) ->bool {
//            return true;
//        });
//    }
//    void FindFit_Multithread(std::function<bool(const Stock& stock)> stock_filter) {
//        ThreadPool pool(16);
//        std::vector<std::future<std::unique_ptr<FitInfo>>> results;
//        for (auto& s : m.stocks) {
//            if (!stock_filter(*s))
//                continue;
//            if (s->trade_data().size() > 10
//                    && s->id()[0] != '3'
//               ) {
//                results.emplace_back(
//                pool.enqueue([&]()-> std::unique_ptr<FitInfo> {
//                    std::unique_ptr<FitInfo> ss(new FitInfo(*s));
//                    int date_index = s->GetIndex(date);
//                    if (date_index == -1)
//                        return std::move(ss);
//                    if (date_index < 5)
//                        return std::move(ss);
//                    if (s->trade_data()[date_index].percent_change() > 9.0 / 100)
//                        return std::move(ss);
//                    const MTI & t = s->mti()[date_index];
//
//                    std::cout << "find fit:" << s->id() << "\n";
//                    if (this->fit_n == 3)
//                        ss->similar_feature_stock_indexes = std::move(t.FindFit3(m));
//                    else
//                        ss->similar_feature_stock_indexes = std::move(t.FindFit2(m));
//                    return std::move(ss);
//                })
//                );
//            }
//        }
//        for (auto && r : results) {
//            stock_statistics.push_back(r.get());
//        }
//    }
//    void FindFit(std::function<bool(const Stock& s)> stock_filter) {
//        /* int date_index = s.GetIndex(date);
//        				if (date_index == -1)
//        					continue;
//        				if (s.trade_data()[date_index].percent_change() > 9.0 / 100)
//        					continue;
//        				const MTI & t = s.mti()[date_index];
//        				StockStatistics ss(s);
//        				std::cout << "find fit:" << s.id() << "\n";
//        				ss.similar_feature_stock_indexes = std::move(t.FindFit(m));
//        				stock_statistics.push_back(ss);*/
//
//    }
//    void MyRound(double* value) {
//        *value = round(*value * 10000.0) / 10000.0;
//    }
//    double MyRound4(double value) {
//        return round(value * 10000.0) / 10000.0;
//    }
//    double MyRound2(double value) {
//        return round(value * 100.0) / 100.0;
//    }
//    void SaveToDb() {
//        year_month_day_hour_min_sec now;
//        std::string str = now.ToString("%Y%m%d %H%M%S");
//        std::size_t str_hash = std::hash<std::string> {}(str);
//        std::string folder = "D:\\Data Center\\MTI" + std::to_string(fit_n) + "-" + date.ToString("%Y%m%d") + " " + std::to_string(str_hash);
//        fs::create_directory(folder);
//
//        SQLiteDB db(folder + "\\total.db");
//        db.CreateTable("CREATE TABLE IF NOT EXISTS stocks(id TEXT,change REAL,samples INTEGER,up_prob REAL,avg_up_percent REAL,avg_down_percent REAL,E REAL,J REAL,delta_J REAL,VR REAL,real_up REAL,comments TEXT);");
//        db.TransactionBegin();
//        //                                                                                                                            1,2,3,4,5,6,7,8,9,0
//        std::string query = "INSERT INTO stocks(id,change,samples,up_prob,avg_up_percent,avg_down_percent,E,J,delta_J,VR,real_up) VALUES(?,?,?,?,?,?,?,?,?,?,?);";
//        db.PrepareInsert(query);
//        for (auto& ss : stock_statistics) {
//            int date_index = ss->stock.GetIndex(this->date);
//            if (date_index == -1) {
//                std::cout << "-1";
//                continue;
//            }
//            std::string ida = ss->stock.id() + " " + ss->stock.name() + " " + std::to_string((int)ss->stock.pe());
//            double change = ss->stock.trade_data()[date_index].percent_change();
//
//            std::wstring id(ida.begin(), ida.end());
//            if ('0' <= ida[0] && ida[0] <= '9') {
//                id = Utf82Unicode(ida);
//            } else {
//                id = Acsi2WideByte(ida);
//            }
//
//            //id = ASCII2UTF_8(id);
//            int samples = ss->similar_feature_stock_indexes.size();
//            if (samples == 0) {
//                continue;
//            }
//            double up_prob = ss->up_probability();
//            double avg_up_percent = ss->average_up();
//            double avg_down_percent = ss->average_down();
//            double E = ss->average_up() * ss->up_probability() + ss->average_down() * (1 - ss->up_probability());
//
//            double J = ss->stock.kdj()[date_index].j;
//            double deltaJ = ss->stock.kdj()[date_index].j - ss->stock.kdj()[date_index - 1].j;
//            double vr = ss->stock.vr()[date_index];
//
//            double real_up = 0.0;
//            if (date != ss->stock.trade_data().back().begin_time) {
//                real_up = ss->stock.trade_data()[date_index + 1].percent_change();
//            }
//
//            MyRound(&up_prob);
//            MyRound(&avg_up_percent);
//            MyRound(&avg_down_percent);
//            MyRound(&E);
//            //std::cout<<"indest "
//            db.InsertBindAndStep(id, MyRound4(change) * 100, samples, up_prob * 100, avg_up_percent * 100, avg_down_percent * 100, E * 100, MyRound2(J), MyRound2(deltaJ), MyRound2(vr), MyRound4(real_up) * 100);
//            // auto query = fmt::format("INSERT INTO stocks(id,samples,up_prob,avg_up_percent,avg_down_percent,E) VALUES('{0}',{1},{2},{3},{4},{5});",
//            //                          id, samples, up_prob * 100, avg_up_percent * 100, avg_down_percent * 100, E * 100);
//            //std::cout << query << std::endl;
//            //db.Insert(query);
//        }
//        db.TransactionCommit();
//        SaveDetails(folder);
//    }
//    void SaveDetails(const std::string& folder) {
//        SQLiteDB db(fmt::format("{0}\\detail.db", folder));
//        db.CreateTable("CREATE TABLE IF NOT EXISTS data(id TEXT,s_id TEXT,date TEXT,change_prev REAL,change REAL,open REAL,high REAL,low REAL);");
//        db.SynchronousOff();
//        db.TransactionBegin();
//        std::string query = "INSERT INTO data(id,s_id,date,change_prev,change,open,high,low) VALUES(?,?,?,?,?,?,?,?);";
//        db.PrepareInsert(query);
//        for (auto& ss : stock_statistics) {
//            if (ss->WillSaveDetail()) {
//                std::cout << "insert detail " << ss->stock.id() << "\n";
//                for (auto& f : ss->similar_feature_stock_indexes) {
//                    std::string similar_id = f.stock->id();
//                    std::string date = f.stock->GetDate(f.i + 1).ToString();
//                    double change_prev = f.stock->trade_data()[f.i].percent_change();
//                    double change = f.stock->trade_data()[f.i + 1].percent_change();
//
//                    double open = (f.stock->trade_data()[f.i + 1].open - f.stock->trade_data()[f.i].close) / f.stock->trade_data()[f.i].close;
//                    double low = (f.stock->trade_data()[f.i + 1].low - f.stock->trade_data()[f.i].close) / f.stock->trade_data()[f.i].close;
//                    double high = (f.stock->trade_data()[f.i + 1].high - f.stock->trade_data()[f.i].close) / f.stock->trade_data()[f.i].close;
//                    // auto query = fmt::format("INSERT INTO data(id, s_id, date, change) VALUES('{0}','{1}','{2}',{3});",
//                    //                          ss.stock.id(), id, date, change);
//                    db.InsertBindAndStep(Acsi2WideByte(ss->stock.id()), Acsi2WideByte(similar_id), date, MyRound4(change_prev) * 100, MyRound4(change) * 100, MyRound4(open) * 100, MyRound4(high) * 100, MyRound4(low) * 100);
//                }
//
//                //ss.SaveToDb(folder);
//            }
//        }
//        db.TransactionCommit();
//    }
//};

