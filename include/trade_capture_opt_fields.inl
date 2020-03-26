#ifndef FIELD
#  error You need to define FIELD macro
#else
FIELD(symbol, 1, 1)
FIELD(trade_publish_ind, 3, 32)
#undef FIELD
#endif
