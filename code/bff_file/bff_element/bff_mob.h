#ifndef __BFFMOB__
#define __BFFMOB__

typedef struct bffmob_s
{
	uint32_t mobtemplate;
	entity_spawner_t* spawner;
	
	const char* mobname;
	uint32_t health;
	uint16_t armor;
	bool hasmelee;
	bool hashitscan;
	bool hasprojectile;
	uint32_t meleedmg, meleerng;
	uint32_t hitscandmg, hitscanrange, hitscanrng;
	uint32_t projectiledmg, projectilerange, projectilerng;
} bffmob_t;

#endif