#include "ASI.h"
#include "Stock.h"

//ASI::ASI(Stock* s) {
//    this->s = s;
//    asis = new double[s->trade_data().size()];
//    asis[0] = 0.0;
//    for (size_t i = 1; i < s->trade_data().size(); i++) {
//        auto today = s->trade_data()[i];
//        auto before = s->trade_data()[i - 1];
//        double A = abs(today.high - before.close);
//        double B = abs(today.low - before.close);
//        double C = abs(today.high - before.low);
//        double D = abs(before.close - before.open);
//        double E = today.close - before.close;
//        double F = today.close - today.open;
//        double G = before.close - before.open;
//        double R;
//        if (A > B && A > C) {
//            R = A + B / 2 + D / 4;
//        } else if (B > A && B > C) {
//            R = B + A / 2 + D / 4;
//        } else {
//            R = C + D / 4;
//        }
//        double X = E + F / 2 + G;
//        double K = A > B ? A : B;
//        double L = 3.0;
//        double SI = 50 * X / R * K / L;
//        asis[i] = asis[i - 1] + SI;
//        //std::cout << i << " " << asis[i] << " " << this->s->GetDateByIndex(i) << std::endl;
//    }
//}
//bool ASI::IsLeadingBreakthrough(eight_digit_time digit_time, int nDays) {
//    int date_index = s->GetIndex(digit_time);
//    if (date_index == -1) {
//        return false;
//    }
//    double today = asis[date_index];
//    if (date_index - nDays <= 0)
//        return false;
//    for (int i = date_index - 1; i > date_index - nDays; i--) {
//        if (today < asis[i])
//            return false;
//    }
//    int end_index = s->GetIndex(digit_time);
//    int begin_index = end_index - nDays;
//    if (s->GetHighestDate(begin_index, end_index) == digit_time) {
//        return false;
//    } else {
//        return true;
//    }
//}
//ASI::~ASI() {
//    if (asis != nullptr) {
//        delete[] asis;
//        asis = nullptr;
//    }
//}

ASICalculator::ASICalculator(const Stock& s, int n) : s(s) {
    if (s.trade_data().size() == 0)
        return;
    si = new float[s.trade_data().size()];
    asi = new float[s.trade_data().size()];
    si[0] = 0.0;
    asi[0] = 0.0;
    for (size_t i = 1; i < s.trade_data().size(); i++) {
        auto today = s.trade_data()[i];
        auto before = s.trade_data()[i - 1];
        double A = abs(today.high - before.close);
        double B = abs(today.low - before.close);
        double C = abs(today.high - before.low);
        double D = abs(before.close - before.open);
        double E = today.close - before.close;
        double F = today.close - today.open;
        double G = before.close - before.open;
        double R;
        if (A > B && A > C) {
            R = A + B / 2 + D / 4;
        } else if (B > A && B > C) {
            R = B + A / 2 + D / 4;
        } else {
            R = C + D / 4;
        }
        double X = E + F / 2 + G;
        double K = A > B ? A : B;
        double L = 3.0;
        double SI = 50 * X / R * K / L;
        si[i] = SI;
        //std::cout << i << " " << asis[i] << " " << this->s->GetDateByIndex(i) << std::endl;
    }
    indicator::SUM(asi, si, n, s.trade_data().size());
}

std::vector<float> ASICalculator::GetResult() {
    std::vector<float> ret;
    for (size_t i = 0; i < s.trade_data().size(); ++i) {
        ret.push_back(asi[i]);
    }
    return std::move(ret);
}