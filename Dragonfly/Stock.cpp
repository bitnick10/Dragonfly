//#include "Stock.h"
//#include "ASI.h"
//#include "KDJ.h"
//#include "MFI.h"
//#include "MTI.h"
//#include "FMTI.h"
//#include "VR.h"
//#include "RSI.h"
//#include "BOLL.h"
//#include "MACD.h"
//
////MFI Stock::GetMFI() {
////    return MFI(this);
////}
//
//void Stock::Indicators::InitDMI(int n1, int n2) {
//    DMICalculator c(*stock, n1, n2);
//    dmi_ = std::move(c.Calculate());
//}
//void Stock::Indicators::InitMA() {
//    if (stock->trade_data().size() < 10)
//        return;
//    ma5_.resize(stock->trade_data().size());
//    ma10_.resize(stock->trade_data().size());
//    ma20_.resize(stock->trade_data().size());
//    ma60_.resize(stock->trade_data().size());
//    mavol10_.resize(stock->trade_data().size());
//    //for (size_t i = 0; i < stock->trade_data().size(); i++) {
//    //    ma5_.push_back(0.0);
//    //    ma60_.push_back(0.0);
//    //}
//    for (size_t i = 0; i < stock->trade_data().size(); i++) {
//        ma5_[i] = indicator::MA(stock->trade_data()[i], 5);
//        ma10_[i] = indicator::MA(stock->trade_data()[i], 10);
//        ma20_[i] = indicator::MA(stock->trade_data()[i], 20);
//        ma60_[i] = indicator::MA(stock->trade_data()[i], 60);
//        mavol10_[i] = indicator::MAVolume(stock->trade_data()[i], 10);
//    }
//    assert(ma5_.size() == stock->trade_data().size());
//    assert(ma10_.size() == stock->trade_data().size());
//    assert(ma20_.size() == stock->trade_data().size());
//    assert(ma60_.size() == stock->trade_data().size());
//    assert(mavol10_.size() == stock->trade_data().size());
//}
//void Stock::Indicators::InitKDJ(int n) {
//    if (stock->trade_data().size() < 10)
//        return;
//
//    double* ks = new double[stock->trade_data().size()];
//    double* ds = new double[stock->trade_data().size()];
//    double* js = new double[stock->trade_data().size()];
//    kdj_.reserve(stock->trade_data().size());
//    // K
//    ks[0] = 50.0;
//    for (size_t i = 1; i < stock->trade_data().size(); i++) {
//        ks[i] = ks[i - 1] * 2 / 3 + indicator::RSV(*stock, i, n) / 3;
//        //std::cout <<  kdjs[i].k << " " << RSV(s, i, n) << std::endl;
//    }
//    // D
//    ds[0] = 50.0;
//    for (size_t i = 1; i < stock->trade_data().size(); i++) {
//        ds[i] = ds[i - 1] * 2 / 3 + ks[i] / 3;
//    }
//    // http://baike.baidu.com/link?url=8ncyymqg-YHvM6Uiz5kEfJwiH85pqHu_TCFN9MQIxT_JogX5Q0j7SAJytmJHW-u8
//    // http://www.wisestocktrader.com/ kdjs/320-stochastic-j-kdj
//    // J = 3K �� 2D
//    for (size_t i = 0; i < stock->trade_data().size(); i++) {
//        js[i] = ks[i] * 3 - ds[i] * 2;
//    }
//    for (size_t i = 0; i < stock->trade_data().size(); i++) {
//        kdj_.push_back(std::move(KDJ(ks[i], ds[i], js[i])));
//    }
//    delete[] ks;
//    delete[] ds;
//    delete[] js;
//}
//Stock::Stock(const std::string & id, const std::string & name, TradeDataType dt, const eight_digit_time& beginDate, const eight_digit_time& endDate) {
//    this->id_ = id;
//    this->name_ = name;
//    this->trade_data_type_ = dt;
//    LoadData(beginDate, endDate);
//
//    indicators_.stock = this;
//    UpdateIndicators();
//    //indicators_.Update();
//}
//void Stock::Indicators::Update() {
//    if (stock->trade_data().size() < 10) {
//        return;
//    }
//    //std::cout << "init " << id << " indicator data\n";
//    macd_.clear();
//    rsi5_.clear();
//    rsi10_.clear();
//    arbr_.clear();
//    kdj_.clear();
//    vr_.clear();
//    asi_.clear();
//    wave_points_.clear();
//    // mti3_.clear();
//    // mti2_.clear();
//    // fmti3_.clear();
//    // fmti2_.clear();
//    ma5_.clear();
//    // ma10_.clear();
//    // ma20_.clear();
//    ma60_.clear();
//
//    InitKDJ(8);
//    InitMA();
//
//    MACDCalculator macdcal(*stock, 12, 26, 9);
//    macd_ = std::move(macdcal.GetResult());
//
//    RSICalculator rsic5(*stock);
//    rsi5_ = std::move(rsic5.GetResult(5));
//
//    RSICalculator rsic10(*stock);
//    rsi10_ = std::move(rsic10.GetResult(10));
//
//    ARBRCalculator arbrc(*stock);
//    arbr_ = std::move(arbrc.GetResult());
//
//    WaveLCalculator wavecal(*stock);
//    wave_points_ = wavecal.GetResult();
//    //InitDMI(14, 6);
//    // init VR
//    VRCalculator vrc(*stock, 26);
//    vr_ = std::move(vrc.Calculate());
//
//    BOLLCalculator bc(*stock, 20, 2);
//    boll_ = std::move(bc.GetResult());
//
//    ASICalculator asic(*stock, 26);
//    asi_ = std::move(asic.GetResult());
//
//    /* mti3_.reserve(stock->trade_data().size());
//     mti3_.push_back(std::move(MTI3()));
//     mti3_.push_back(std::move(MTI3()));
//     mti3_.push_back(std::move(MTI3()));
//     mti3_.push_back(std::move(MTI3()));
//     mti3_.push_back(std::move(MTI3()));
//     for (size_t i = 5; i < stock->trade_data().size(); i++) {
//         mti3_.push_back(std::move(MTI3(stock->trade_data()[i], *stock)));
//     }
//     assert(mti3_.size() == stock->trade_data().size());
//
//     mti2_.reserve(stock->trade_data().size());
//     mti2_.push_back(std::move(MTI2()));
//     mti2_.push_back(std::move(MTI2()));
//     mti2_.push_back(std::move(MTI2()));
//     mti2_.push_back(std::move(MTI2()));
//     mti2_.push_back(std::move(MTI2()));
//     for (size_t i = 5; i < stock->trade_data().size(); i++) {
//         mti2_.push_back(std::move(MTI2(stock->trade_data()[i], *stock)));
//     }
//     assert(mti2_.size() == stock->trade_data().size());*/
//
//    /* fmti3_.reserve(stock->trade_data().size());
//     fmti3_.push_back(std::move(FMTI3()));
//     fmti3_.push_back(std::move(FMTI3()));
//     fmti3_.push_back(std::move(FMTI3()));
//     fmti3_.push_back(std::move(FMTI3()));
//     fmti3_.push_back(std::move(FMTI3()));
//     for (size_t i = 5; i < stock->trade_data().size(); i++) {
//         fmti3_.push_back(std::move(FMTI3(stock->trade_data()[i], *stock)));
//     }
//     assert(fmti3_.size() == stock->trade_data().size());
//
//     fmti2_.reserve(stock->trade_data().size());
//     fmti2_.push_back(std::move(FMTI2()));
//     fmti2_.push_back(std::move(FMTI2()));
//     fmti2_.push_back(std::move(FMTI2()));
//     fmti2_.push_back(std::move(FMTI2()));
//     fmti2_.push_back(std::move(FMTI2()));
//     for (size_t i = 5; i < stock->trade_data().size(); i++) {
//         fmti2_.push_back(std::move(FMTI2(stock->trade_data()[i], *stock)));
//     }
//     assert(fmti2_.size() == stock->trade_data().size());*/
//}
//
////Stock::Stock(const Stock& s) : id_(s.id_), name_(s.name_), trade_data_type_(s.trade_data_type_) {
////    //trade_data_ = s.trade_data_;
////    //if (s.mti_ == nullptr)
////    //    this->mti_ = nullptr;
////    //else {
////    //    this->mti_ = new MTI[trade_data().size()];
////    //    memcpy(this->mti_, s.mti_, trade_data().size() * sizeof(MTI));
////    //}
////    // kdj = new KDJ(this, 3);
////}
////Stock::Stock(Stock&& s):
////    id_(std::move(s.id_)),
////    name_(std::move(s.name_)),
////    trade_data_type_(s.trade_data_type_),
////    trade_data_(std::move(s.trade_data_)),
////    mti_(std::move(s.mti_)),
////    kdj_(std::move(s.kdj_)),
////    vr_(std::move(s.vr_)) {
////    s.arbr = nullptr;
////    this->pe_ = s.pe_;
////}
//
//Stock::~Stock() {
//
//}