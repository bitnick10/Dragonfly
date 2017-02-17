#pragma once

#include <iostream>
#include <vector>

class Chart;

struct MACD {
    float macd, diff, dea;
};

class MACDCalculator {
public:
    static std::vector<MACD> GetResult(const Chart& chart, int short_n, int long_n, int dea_n);
};

struct KDJ {
public:
    float k, d, j;
};

class KDJCalculator {
public:
    static std::vector<KDJ> GetResult(const Chart& chart, int n, int sn1, int sn2);
};