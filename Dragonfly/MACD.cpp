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

std::vector<KDJ> KDJCalculator::GetResult(const Chart& chart, int n, int sn1, int sn2) {
    std::vector<KDJ> ret;
    if (chart.sticks().size() < 10)
        return ret;
    ret.resize(chart.sticks().size());
    auto& rsvs = chart.RawStochasticValues(n);
    auto& ks = indicator::SMA(rsvs, sn1, 1.0);
    auto& ds = indicator::SMA(ks, sn2, 1.0);
    for (size_t i = 0; i < ret.size(); i++) {
        ret[i].k = ks[i];
        ret[i].d = ds[i];
        ret[i].j = ks[i] * 3 - ds[i] * 2;
    }
    return ret;
}

