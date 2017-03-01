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
