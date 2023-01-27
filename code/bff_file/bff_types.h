#ifndef __BFFTYPES__
#define __BFFTYPES__

#define bffenum uint_fast8_t

typedef __int128_t magicnum_t;
typedef bffenum byte;
typedef byte buffer[1024];

#endif

#ifndef __BFFDEFS__
#define __BFFDEFS__

#define NotFound std::string::npos
#define BFFVAR static constexpr

BFFVAR magicnum_t BFF_FILE_HEADER = 0xa49d21f;

BFFVAR uint8_t CONTEXT_NO_CHUNK   = 0b00000000;
BFFVAR uint8_t CONTEXT_MAP_CHUNK  = 0b00000001;
BFFVAR uint8_t CONTEXT_DATA_CHUNK = 0b00000010;

BFFVAR uint32_t BFF_UNDEFINED      = 0x0000;
BFFVAR uint32_t BFF_TYPE_CAMPAIGN  = 0x0001;
BFFVAR uint32_t BFF_TYPE_ROGUELIKE = 0x0002;
BFFVAR uint32_t BFF_TYPE_DOOMCLONE = 0x0003;
BFFVAR uint32_t BFF_DATA_ITEM      = 0x0004;
BFFVAR uint32_t BFF_DATA_WPN       = 0x0005;
BFFVAR uint32_t BFF_DATA_MOB       = 0x0006;
BFFVAR uint32_t BFF_DATA_NPC       = 0x0007;
BFFVAR uint32_t BFF_MAX_ALLOWED    = 0x0008;

#endif