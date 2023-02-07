#ifndef __BFFDATA__
#define __BFFDATA__

typedef struct _bffdata
{
	uint32_t dtype;
	std::vector<bffnpc_t> npc_data;
	std::vector<bffmob_t> mob_data;
	std::vector<bffitem_t> item_data;
	std::vector<bffwpn_t> wpn_data;
} bffdata_t;

#endif