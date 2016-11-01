#pragma once

#include <vector>

#include "../Dragonfly/StockMarket.h"

#include "MTIHelper.h"

class MTIExplorerHelper {
public:
    static std::vector<OneMTI> GetMTIList(const StockMarket & m, eight_digit_time beginDate, eight_digit_time endDate, std::string mti_name) {
        std::vector<OneMTI> ret;
        for (eight_digit_time date = beginDate; date.value() <= endDate.value(); date = date + 1) {
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
        }
        return ret;
    }
};

class MTIExplorer {
public:
    bool using_multithread = true;
    std::string folder_prefix = "";
public:
    void GenerateReport(const std::vector<OneMTI>& mti_list, const std::string& mti_name, const Market& pool) {
        std::vector<std::unique_ptr<MTIInfo>> results;
        if (using_multithread) {
            ThreadPool thread_pool(16);
            std::vector<std::future<std::unique_ptr<MTIInfo>>> future_results;
            for (auto && one : mti_list) {
                future_results.emplace_back(
                thread_pool.enqueue([&]()-> std::unique_ptr<MTIInfo> {
                    fmt::print("Caculating MTI{0}Info {1} {2}\n", mti_name, one.father->id(), one.date.ToYYYYMMDDString());
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
        std::cout << "save to database" << std::endl;
        SaveToDb(results, mti_name);
    }
    std::string GetRandomFolderPath(const std::string& mti_name) {
        //std::cout << "DEBUG 1" << std::endl;
        year_month_day_hour_min_sec now; // use for generate hash string
        std::string str = now.ToString("%Y%m%d %H%M%S");
        //std::cout << "DEBUG 2" << std::endl;
        std::size_t str_hash = std::hash<std::string> {}(str);
        //std::cout << "DEBUG 3" << std::endl;
        std::string random_hash = std::to_string(str_hash);
        //std::cout << "DEBUG 4" << std::endl;
        std::string folder = fmt::format("D:\\Data Center\\{0} {1} {2}", this->folder_prefix, mti_name, random_hash);
        //std::cout << "DEBUG 5" << std::endl;
        return folder;
    }
    void SaveToDb(const std::vector<std::unique_ptr<MTIInfo>>& results, const std::string& mti_name) {
        std::string folder_path;
        // if (this->is_raeltime) {
        // realtime data create folder WITHOUT hash
        //  folder_path = fmt::format("D:\\Data Center\\{0} {1}", this->folder_prefix, mti_name);
        // } else {
        std::cout << "GetRandomFolderPath" << std::endl;
        folder_path = GetRandomFolderPath(mti_name);
        std::cout << "GetRandomFolderPath OVER" << std::endl;
        // }
        /* if (this->is_raeltime) {
        if (fs::exists(folder_path))
        fs::remove_all(folder_path);
        }*/

        if (!fs::exists(folder_path)) {
            std::cout << "create_directory" << std::endl;
            fs::create_directory(folder_path);
        }

        SQLiteDB db(folder_path + "\\total.db");

        db.CreateTable("CREATE TABLE IF NOT EXISTS stocks(id TEXT,change REAL,samples INTEGER,up_prob REAL,avg_up_percent REAL,avg_down_percent REAL,E REAL,up_avg_high REAL, up_avg_low REAL, down_avg_high REAL, down_avg_low REAL,real_up REAL,comments TEXT);");
        db.ClearTable("stocks");
        db.TransactionBegin();
        //                                                                                                                                               1,2,3,4,5,6,7,8,9,0
        std::string query = "INSERT INTO stocks(id,change,samples,up_prob,avg_up_percent,avg_down_percent,E,up_avg_high, up_avg_low, down_avg_high, down_avg_low,real_up) VALUES(?,?,?,?,?,?,?,?,?,?,?,?);";
        db.PrepareInsert(query);
        for (auto && info : results) {
            year_month_day_hour_min_sec date = info->one_mti.date;
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
        SaveDetails(folder_path, results, mti_name);
    }
    void SaveDetails(const std::string& folder, const std::vector<std::unique_ptr<MTIInfo>>& results, const std::string& mti_name) {
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
                    db.InsertBindAndStep(Acsi2WideByte(info->one_mti.father->id() + info->one_mti.date.ToString()), Acsi2WideByte(similar_id), date, MyRound4(change_prev) * 100, MyRound4(change) * 100, MyRound4(open) * 100, MyRound4(high) * 100, MyRound4(low) * 100);
                }

                //ss.SaveToDb(folder);
            }
        }
        db.TransactionCommit();
    }
private:
    double MyRound4(double value) {
        return round(value * 10000.0) / 10000.0;
    }
    double MyRound2(double value) {
        return round(value * 100.0) / 100.0;
    }
};