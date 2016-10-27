#include "ARBR.h"
#include "Stock.h"
#include "Indicator.h"

ARBRCalculator::ARBRCalculator(const Stock& s) : stock(s) {
}

std::vector<ARBR> ARBRCalculator::GetResult() {
    std::vector<ARBR> ret;
    assert(stock.trade_data().size() > 0);
    if (stock.trade_data().size() == 0) {
        return ret;
    }
    double * arday1 = new double[stock.trade_data().size()];
    double * arday2 = new double[stock.trade_data().size()];
    double * brday1 = new double[stock.trade_data().size()];
    double * brday2 = new double[stock.trade_data().size()];
    for (size_t i = 0; i < stock.trade_data().size(); i++) {
        arday1[i] = (stock.trade_data()[i].high - stock.trade_data()[i].open);
        arday2[i] = (stock.trade_data()[i].open - stock.trade_data()[i].low);
    }
    brday1[0] = 0.0;
    brday2[0] = 0.0;
    for (size_t i = 1; i < stock.trade_data().size(); i++) {
        brday1[i] = MAX(0, (stock.trade_data()[i].high - stock.trade_data()[i - 1].close));
        brday2[i] = MAX(0, (stock.trade_data()[i - 1].close - stock.trade_data()[i].low));
    }
    ret.resize(stock.trade_data().size());
    for (size_t i = 25; i < stock.trade_data().size(); i++) {
        double sum_h_o = 0.0;
        double sum_o_l = 0.0;
        double bracc0 = 0.0;
        double bracc1 = 0.0;
        for (size_t j = i - 25; j <= i; j++) {
            sum_h_o += arday1[j];
            sum_o_l += arday2[j];
            bracc0 += brday1[j];
            bracc1 += brday2[j];
        }
        ret[i].ar = sum_h_o / sum_o_l * 100;
        ret[i].br = bracc0 / bracc1 * 100;
    }
    for (size_t i = 0; i < 25 && i < stock.trade_data().size(); i++) {
        double h_o = 0.0;
        double o_l = 0.0;
        double bracc0 = 0.0;
        double bracc1 = 0.0;
        for (int j = i; j > 0; j--) {
            h_o += arday1[j];
            o_l += arday2[j];
            bracc0 += brday1[j];
            bracc1 += brday2[j];
        }
        ret[i].ar = h_o / o_l * 100;
        ret[i].br = bracc0 / bracc1 * 100;
    }
    if (arday1 != nullptr) {
        delete[] arday1;
        arday1 = nullptr;
    }
    if (arday2 != nullptr) {
        delete[] arday2;
        arday2 = nullptr;
    }
    if (brday1 != nullptr) {
        delete[] brday1;
        brday1 = nullptr;
    }
    if (brday2 != nullptr) {
        delete[] brday2;
        brday2 = nullptr;
    }
    return ret;
}