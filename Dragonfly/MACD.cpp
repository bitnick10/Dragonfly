#include "MACD.h"
#include "Trade.h"
#include "Indicator.h"

#pragma warning(disable: 4244) // warning C4244: conversion from 'double' to 'float', possible loss of data

std::vector<MACD> MACDCalculator::GetResult(const Chart& chart, int short_n, int long_n, int dea_n) {
    std::vector<MACD> ret;
    assert(chart.sticks().size() > 0);
    if (chart.sticks().size() == 0) {
        return ret;
    }
    std::vector<double> short_emas = indicator::EMA(chart, short_n);
    std::vector<double> long_emas = indicator::EMA(chart, long_n);

    double * diffs = new double[chart.sticks().size()];
    double * deas = new double[chart.sticks().size()];
    double * macd = new double[chart.sticks().size()];

    for (size_t i = 0; i < chart.sticks().size(); i++) {
        diffs[i] = short_emas[i] - long_emas[i];
    }
    indicator::EMA(deas, diffs, chart.sticks().size(), dea_n);
    for (size_t i = 0; i < chart.sticks().size(); i++) {
        macd[i] = 2 * (diffs[i] - deas[i]);
    }
    ret.resize(chart.sticks().size());
    for (size_t i = 0; i < chart.sticks().size(); i++) {
        ret[i].diff = diffs[i];
        ret[i].dea = deas[i];
        ret[i].macd = macd[i];
    }
    if (diffs != nullptr) {
        delete[] diffs;
        diffs = nullptr;
    }
    if (deas != nullptr) {
        delete[] deas;
        deas = nullptr;
    }
    if (macd != nullptr) {
        delete[] macd;
        macd = nullptr;
    }
    return ret;
}

//KDJ::KDJ(const Chart& chart, int n, int sn1, int sn2) {
//    Init(chart, n, sn1, sn2);
//}
void KDJ::Init() {
    if (this->k.size() == 0) {
        this->k.resize(chart->sticks().size());
        this->d.resize(chart->sticks().size());
        this->k.shrink_to_fit();
        this->d.shrink_to_fit();
    }
    //this->j.resize(chart.sticks().size());
    //ret.resize(chart.sticks().size());

    auto& rsvs = chart->RawStochasticValues(n);
    indicator::SMA(this->k, rsvs, sn1, 1.0);
    indicator::SMA(this->d, this->k, sn1, 1.0);
    //for (size_t i = 0; i < chart->sticks().size(); i++) {
    //this->k[i] = ks[i];
    //this->d[i] = ds[i];
    //this->j[i] = ks[i] * 3 - ds[i] * 2;
    //}
}
KDJ::KDJ(const Chart* chart, int n, int sn1, int sn2, bool is_allocate_memory, bool is_init_data) {
    //if (chart.sticks().size() < 10)
    //    assert(false);
    this->chart = chart;
    this->n = n;
    this->sn1 = sn1;
    this->sn2 = sn2;
    if (is_allocate_memory) {
        this->k.resize(chart->sticks().size());
        this->d.resize(chart->sticks().size());
        this->k.shrink_to_fit();
        this->d.shrink_to_fit();
    }
    if (is_init_data) {
        Init();
    }
}

BOLL::BOLL(const Chart* pchart, int n, int p) {
    // if (s.trade_data().size() == 0)
    //     return;
    this->chart = pchart;
    this->n = n;
    this->p = p;
    mid.resize(pchart->sticks().size());
    upper.resize(pchart->sticks().size());
    lower.resize(pchart->sticks().size());
    std::vector<float> sd;
    sd.resize(pchart->sticks().size());
    for (size_t i = 0; i < pchart->sticks().size(); ++i) {
        mid[i] = indicator::MA(*chart, i, n);
    }
    for (size_t i = 3; i < pchart->sticks().size(); ++i) {
        sd[i] = indicator::StandardDeviation(*chart, i, mid[i], n);
    }
    for (size_t i = 0; i < pchart->sticks().size(); ++i) {
        upper[i] = mid[i] + p * sd[i];
        lower[i] = mid[i] - p * sd[i];
    }
}
//bool is_high(const Chart* pchart, size_t queue_i, int n) {
//    int begin_i;
//    int end_i;
//    begin_i = queue_i - n;
//    begin_i = MAX(0, begin_i);
//    end_i = MIN(pchart->sticks().size() - 1, queue_i + n);
//    for (int i = begin_i; i <= end_i; i++) {
//        if (i == queue_i)
//            continue;
//        if (pchart->sticks()[i].high > pchart->sticks()[queue_i].high)
//            return false;
//    }
//    return true;
//}
//bool is_low(const Chart* pchart, size_t queue_i, int n) {
//    int begin_i;
//    int end_i;
//    begin_i = queue_i - n;
//    begin_i = MAX(0, begin_i);
//    end_i = MIN(pchart->sticks().size() - 1, queue_i + n);
//    for (int i = begin_i; i <= end_i; i++) {
//        if (i == queue_i)
//            continue;
//        if (pchart->sticks()[i].low < pchart->sticks()[queue_i].low)
//            return false;
//    }
//    return true;
//}
float POI::GetPos(int data_i) {
    Tag tag1, tag2;
    int it1, it2;
    it1 = NearestTag(data_i);
    assert(it1 != -1);
    tag1 = tags[it1];
    tag2 = OppsiteTag(tag1);
    it2 = NearestTag(data_i, tag2);
    if (it2 == -1)
        return 0.5;
    double wave;
    double close;
    assert(tag1 != Tag::None);
    if (tag1 == Tag::NewLow) {
        wave = chart->sticks()[it2].high - chart->sticks()[it1].low;
        close = chart->sticks()[data_i].close - chart->sticks()[it1].low;
    }
    if (tag1 == Tag::NewHigh) {
        wave = chart->sticks()[it1].high - chart->sticks()[it2].low;
        close = chart->sticks()[data_i].close - chart->sticks()[it1].high;
    }
    // double close = chart->sticks()[data_i].close;
    return close / wave;
}
POI::POI(const Chart* pchart, int n) {
    this->chart = pchart;
    this->n = n;
    tags.resize(pchart->sticks().size());
    pos.resize(pchart->sticks().size());
    for (size_t i = 0; i < pchart->sticks().size(); ++i) {
        tags[i] = Tag::None;
        if (pchart->IsHighNewHigh(i, n))
            tags[i] = Tag::NewHigh;
        if (pchart->IsLowNewLow(i, n))
            tags[i] = Tag::NewLow;
    }
    for (size_t i = 0; i < pchart->sticks().size(); ++i) {
        pos[i] = GetPos(i);
    }
}