#pragma once

#include <iostream>
#include <vector>

#include "BOLL.h"

class Stock;

class RSICalculator {
public:
    double *ups;
    double *downs;
    double *upsma;
    double *downsma;
    double *rs;
    double *rsi;
    const Stock& stock;
    int n;
    RSICalculator(const Stock& stock, int n = 6);
    std::vector<float> GetResult();
    ~RSICalculator() {
        DELETE_IF_NOT_NULLPTR(ups);
        DELETE_IF_NOT_NULLPTR(downs);
        DELETE_IF_NOT_NULLPTR(upsma);
        DELETE_IF_NOT_NULLPTR(downsma);
        DELETE_IF_NOT_NULLPTR(rs);
        DELETE_IF_NOT_NULLPTR(rsi);
    }
};