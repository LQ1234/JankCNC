#include "pair.h"
enum class SPIMsgType : uint8_t {SETPAIR, GETIP, GETPAIR, ECHO};
struct __attribute__((__packed__)) SPIMsgCommand{
    SPIMsgType type; //1 byte
    union {
        Pair setpair; //100 bytes

        struct {} getip;

        struct {} getpair;

        uint8_t echo[100];
    } data;
    uint8_t padding[3];//padding
};

struct  __attribute__((__packed__)) SPIMsgResult{
    SPIMsgType type;
    union {
        struct {} setpair;

        uint32_t getip;

        Pair getpair;

        uint8_t echo[100];
    } data;
};
