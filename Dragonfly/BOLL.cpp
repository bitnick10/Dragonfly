#include "BOLL.h"
#include "Stock.h"
#include "Indicator.h"

BOLLCalculator::BOLLCalculator(const Stock& s, int n, int p): s(s) {
    if (s.trade_data().size() == 0)
        return;
    this->p = p;
    mids = new double[s.trade_data().size()];
    sd = new double[s.trade_data().size()];
    for (size_t i = 0; i < s.trade_data().size(); ++i) {
        mids[i] = indicator::MA(s.trade_data()[i], n);
    }
    for (size_t i = 3; i < s.trade_data().size(); ++i) {
        sd[i] = indicator::StandardDeviation(s.trade_data()[i], mids[i], n);
    }
}

std::vector<BOLL> BOLLCalculator::GetResult() {
    std::vector<BOLL> ret;
    for (size_t i = 0; i < s.trade_data().size(); ++i) {
        BOLL b;
        b.mid = mids[i];
        b.upper = b.mid + p * sd[i];
        b.lower = b.mid - p * sd[i];
        ret.push_back(b);
    }
    return std::move(ret);
}