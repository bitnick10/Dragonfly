#pragma once

#include "../Dragonfly/QStockMarket.h"

#include "QStrategyBase.h"

class QStrategyBase {
public:
    QAccount account;
    QStrategyBase(QStockMarket& market) : account(market) {}
};
