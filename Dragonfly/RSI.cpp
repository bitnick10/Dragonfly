#include "RSI.h"
#include "Stock.h"
#include "Indicator.h"

RSICalculator::RSICalculator(const Stock& s, int n): stock(s) {
    this->n = n;
}

std::vector<float> RSICalculator::GetResult() {
    std::vector<float> ret;
    if (stock.trade_data().size() == 0)
        return ret;

    ups = new double[stock.trade_data().size()];
    downs = new double[stock.trade_data().size()];
    upsma = new double[stock.trade_data().size()];
    downsma = new double[stock.trade_data().size()];
    rs = new double[stock.trade_data().size()];
    rsi = new double[stock.trade_data().size()];
    ups[0] = 0.0;
    downs[0] = 0.0;
    rs[0] = 0.0;
    rsi[0] = 0.0;
    for (size_t i = 1; i < stock.trade_data().size(); i++) {
        float lc = stock.trade_data()[i - 1].close;
        ups[i] = MAX(stock.trade_data()[i].close - lc, 0);
        downs[i] = abs(stock.trade_data()[i].close - lc);
    }
    indicator::SMA(upsma, ups, stock.trade_data().size(), n, 1);
    indicator::SMA(downsma, downs, stock.trade_data().size(), n, 1);
    for (size_t i = 1; i < stock.trade_data().size(); i++) {
        rsi[i] = upsma[i] / downsma[i] * 100;
        //rsi[i] = 100 * rs[i] / (1 + rs[i]);
    }

    for (size_t i = 0; i < stock.trade_data().size(); ++i) {
        ret.push_back(rsi[i]);
    }
    return std::move(ret);
}