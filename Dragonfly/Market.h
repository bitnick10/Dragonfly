#pragma once

#include "Futures.h"

class Market {
public:
    std::vector<std::unique_ptr<Chart>> charts;
};