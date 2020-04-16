#ifndef FIELD
#  error You need to define FIELD macro
#else
FIELD(capacity, 2, 1)
FIELD(party_role, 2, 16)
#undef FIELD
#endif