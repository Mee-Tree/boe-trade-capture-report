#ifndef FIELD
#  error You need to define FIELD macro
#else
FIELD(symbol, 1, 1)
FIELD(capacity, 2, 1)
FIELD(account, 2, 2)
FIELD(party_role, 2, 16)
FIELD(trade_publish_ind, 3, 32)
#undef FIELD
#endif
