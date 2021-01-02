#include "requests.h"

namespace {

void encode_new_order_opt_fields(unsigned char * bitfield_start,
        const double price,
        const char ord_type,
        const char time_in_force,
        const unsigned max_floor,
        const std::string & symbol,
        const char capacity,
        const std::string & account)
{
    auto * p = bitfield_start + new_order_bitfield_num();
#define FIELD(name, bitfield_num, bit) \
    set_opt_field_bit(bitfield_start, bitfield_num, bit); \
    p = encode_field_##name(p, name);
#include "new_order_opt_fields.inl"
}


void encode_trade_capture_opt_fields(unsigned char * bitfield_start, unsigned char * p,
        const std::string & symbol,
        const char trade_publish_ind)
{
#define FIELD(name, bitfield_num, bit) \
    set_opt_field_bit(bitfield_start, bitfield_num, bit); \
    p = encode_field_##name(p, name);
#include "trade_capture_opt_fields.inl"
}

unsigned char * encode_trade_capture_group_opt_fields(unsigned char * bitfield_start, unsigned char * p,
        const char capacity,
        const char party_role)
{
#define FIELD(name, bitfield_num, bit) \
    set_opt_field_bit(bitfield_start, bitfield_num, bit); \
    if (std::strcmp(#name, "capacity") == 0) {\
        p -= capacity_field_size + party_id_field_size;\
        p = encode_field_capacity(p, name) + party_id_field_size;\
    } else p = encode_field_##name(p, name);
#include "trade_capture_group_opt_fields.inl"
    return p;
}

uint8_t encode_request_type(const RequestType type)
{
    switch (type) {
        case RequestType::New:
            return 0x38;
        case RequestType::TradeCapture:
            return 0x3C;
    }
    return 0;
}

unsigned char * add_request_header(unsigned char * start, unsigned length, const RequestType type, unsigned seq_no)
{
    *start++ = 0xBA;
    *start++ = 0xBA;
    start = encode(start, static_cast<uint16_t>(length));
    start = encode(start, encode_request_type(type));
    *start++ = 0;
    return encode(start, static_cast<uint32_t>(seq_no));
}

char convert_side(const Side side)
{
    switch (side) {
        case Side::Buy: return '1';
        case Side::Sell: return '2';
    }
    return 0;
}

char convert_ord_type(const OrdType ord_type)
{
    switch (ord_type) {
        case OrdType::Market: return '1';
        case OrdType::Limit: return '2';
        case OrdType::Pegged: return 'P';
    }
    return 0;
}

char convert_time_in_force(const TimeInForce time_in_force)
{
    switch (time_in_force) {
        case TimeInForce::Day: return '0';
        case TimeInForce::IOC: return '3';
        case TimeInForce::GTD: return '6';
    }
    return 0;
}

char convert_capacity(const Capacity capacity)
{
    switch (capacity) {
        case Capacity::Agency: return 'A';
        case Capacity::Principal: return 'P';
        case Capacity::RisklessPrincipal: return 'R';
    }
    return 0;
}

} // anonymous namespace

std::array<unsigned char, calculate_size(RequestType::New)> create_new_order_request(const unsigned seq_no,
        const std::string & cl_ord_id,
        const Side side,
        const double volume,
        const double price,
        const OrdType ord_type,
        const TimeInForce time_in_force,
        const double max_floor,
        const std::string & symbol,
        const Capacity capacity,
        const std::string & account)
{
    static_assert(calculate_size(RequestType::New) == 78, "Wrong New Order message size");

    std::array<unsigned char, calculate_size(RequestType::New)> msg;
    auto * p = add_request_header(&msg[0], msg.size() - 2, RequestType::New, seq_no);
    p = encode_text(p, cl_ord_id, 20);
    p = encode_char(p, convert_side(side));
    p = encode_binary4(p, static_cast<uint32_t>(volume));
    p = encode(p, static_cast<uint8_t>(new_order_bitfield_num()));
    encode_new_order_opt_fields(p,
            price,
            convert_ord_type(ord_type),
            convert_time_in_force(time_in_force),
            max_floor,
            symbol,
            convert_capacity(capacity),
            account);
    return msg;
}

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
        bool deferred_publication)
{
    std::vector<unsigned char> msg(calculate_size(RequestType::TradeCapture));
    auto * p = add_request_header(&msg[0], msg.size() - 2, RequestType::TradeCapture, seq_no);
    
    p = encode_text(p, trade_report_id, 20);
    p = encode_binary4(p, static_cast<uint32_t>(volume));
    p = encode_trade_price(p, price);
    p = encode(p, static_cast<uint8_t>(trade_capture_bitfield_num()));
    
    auto * bitfield_start = p;
    p += trade_capture_bitfield_num();
    
    uint8_t no_sides = 2;
    p = encode(p, no_sides);

    Side contra_side = side == Side::Buy ? Side::Sell : Side::Buy;

    char party_roles[] = {'2', '3'};
    Side sides[] = {side, contra_side};
    Capacity capacities[] = {capacity, contra_capacity};
    std::string party_ids[] = {party_id, contra_party_id};

    for (auto i = 0; i < no_sides; ++i) {
        p = encode_char(p, convert_side(sides[i])); // mandatory
        p += capacity_field_size;
        p = encode_field_party_id(p, party_ids[i]); // mandatory
        p = encode_trade_capture_group_opt_fields(bitfield_start, p,
              convert_capacity(capacities[i]),
              party_roles[i]
        );
    }

    char trade_publish_ind = static_cast<char>(deferred_publication + 1);
    encode_trade_capture_opt_fields(bitfield_start, p,
        symbol,
        trade_publish_ind);
    return msg;
}
