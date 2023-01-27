#ifndef __BFFNPC__
#define __BFFNPC__

typedef struct bffnpc_s
{
	uint32_t npctemplate;
	entity_spawner_t* spawner;
	
	std::string npcname;
	uint32_t health;
	uint16_t armor;
} bffnpc_t;

#endif