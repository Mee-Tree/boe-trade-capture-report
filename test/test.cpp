#include "requests.h"

#include <gtest/gtest.h>

TEST(NewOrderTest, simple_size_check)
{
    const auto new_order_msg = create_new_order_request(1,
            "ORD1001",
            Side::Buy,
            100,
            12.505,
            OrdType::Limit,
            TimeInForce::Day,
            10,
            "AAPl",
            Capacity::Principal,
            "ACC331");
    EXPECT_EQ(100, new_order_msg.size());
}
