#include "gtest/gtest.h"

#include "../Dragonfly/Stock.h"
#include "../Dragonfly/Market.h"
#include "../Dragonfly/KDJ.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}