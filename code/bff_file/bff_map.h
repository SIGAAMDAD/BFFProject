#ifndef __BFFMAP__
#define __BFFMAP__

typedef struct bffmap_s
{
	std::string mapname;
	char sectormap[9][120][120];
	int8_t sectorcounter;
} bffmap_t;

#endif