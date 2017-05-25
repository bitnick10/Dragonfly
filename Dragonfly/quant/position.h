#include "../DateTime.h"

namespace quant {
struct Position {
    enum Type {
        Long = 1,
        Short = -1
    };
    Type type = Type::Long;
    double create_price;
    double close_price;
    DateTime create_time;
    DateTime close_time;
    double profit_percent() const {
        return (close_price - create_price) / create_price * type;
    }
    double profit_percent(double price) const {
        return (price - create_price) / create_price * type;
    }
    double get_take_profit_price(double percent) const {
        assert(percent > 0.0);
        return create_price * (1 + percent * type) ;
    }
    double get_stop_loss_price(double percent) const {
        assert(percent < 0.0);
        return create_price * (1 + percent * type);
    }
    bool will_take_profit(double low, double high, double percent) const {
        assert(percent > 0.0);
        if (profit_percent(low) > percent || profit_percent(high) > percent)
            return true;
        return false;
    }
    bool will_stop_loss(double low, double high, double percent) const {
        assert(percent < 0.0);
        if (profit_percent(low) < percent || profit_percent(high) < percent)
            return true;
        return false;
    }
};
}