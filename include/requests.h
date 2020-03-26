#pragma once

#include "fields.h"

#include <algorithm>
#include <array>
#include <vector>

/*
 * New Order
 *  Price(1,4)
 *  OrdType(1,16)
 *  TimeInForce(1,32)
 *  MaxFloor(1,128)
 *  Symbol(2,1)
 *  Capacity(2,64)
 *  Account(3,1)
 */

constexpr size_t new_order_bitfield_num()
{
    return std::max({0
#define FIELD(_, n, __) , n
#include "new_order_opt_fields.inl"
        });
}

constexpr size_t new_order_opt_fields_size()
{
    return 0
#define FIELD(name, _, __) + name##_field_size
#include "new_order_opt_fields.inl"
        ;
}

enum class RequestType {
    New,
    TradeCapture
};

constexpr size_t calculate_size(const RequestType type)
{
    switch (type) {
        case RequestType::New:
            return 36 + new_order_bitfield_num() + new_order_opt_fields_size();
        case RequestType::TradeCapture:
            return 43 + new_order_bitfield_num() + new_order_opt_fields_size();
    }
    return 0;
}

enum class Side {
    Buy,
    Sell
};

enum class OrdType {
    Market,
    Limit,
    Pegged
};

enum class TimeInForce {
    Day,
    IOC,
    GTD
};

enum class Capacity {
    Agency,
    Principal,
    RisklessPrincipal
};

std::array<unsigned char, calculate_size(RequestType::New)> create_new_order_request(
        unsigned seq_no,
        const std::string & cl_ord_id,
        Side side,
        double volume,
        double price,
        OrdType ord_type,
        TimeInForce time_in_force,
        double max_floor,
        const std::string & symbol,
        Capacity capacity,
        const std::string & account);

std::vector<unsigned char> create_trade_capture_report_request(
        unsigned seq_no,
        const std::string & trade_report_id,
        double volume,
        double price,
        const std::string & party_id,
        Side side,
        Capacity capacity,
        const std::string & contra_party_id,
        Capacity contra_capacity,
        const std::string & symbol,
        bool deferred_publication);
