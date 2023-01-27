#include "BFF.h"

BFF_API void BFF_ParseMapElement(bff_file_t* file)
{
}
BFF_API void BFF_ParseItemElement(bff_file_t* file)
{
}
BFF_API void BFF_ParseWpnElement(bff_file_t* file)
{
}
BFF_API void BFF_ParseMobElement(bff_file_t* file)
{
}

BFF_API void BFF_ParseNPCElement(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	bffdata_t* data = file->c_data;
	data->npc_data.emplace_back();
	bff_runtime_t* r = file->run;
	bffnpc_t* const npc = &data->npc_data.back();
	npc->npctemplate = BFF_UNDEFINED;
	for (;; ++r->i) {
		if (r->tokbuf[r->i].ftok == tokens[ELEMENT_DECLARE]
		&& r->tokbuf[r->i].btok == tokens[DATA_CLOSE]) { break; }
		const std::string& strbuf = r->tokbuf[r->i].buffer;
		switch (r->tokbuf[r->i].ftok) {
		case tokens[PROP_DECLARE]: {
			for (;; ++r->i) {
				if (r->tokbuf[r->i].ftok == tokens[PROP_DECLARE]
				&& r->tokbuf[r->i].btok == tokens[DATA_CLOSE]) { break; }
				const std::string& buf = r->tokbuf[r->i].buffer;
				if (buf.find("NAME") != NotFound) {
					npc->npcname = r->tokbuf[r->i+1].buffer;
				} else if (buf.find("TEMPLATE") != NotFound) {
					BFF_GetNumU32(&npc->npctemplate, r->tokbuf[r->i+1].buffer, (r->i+1));
				}
			}
			break; }
		case tokens[SPECIAL_VAR_DECLARE]:
			if (npc->npctemplate != BFF_UNDEFINED)
				BFF_ThrowError(r->i, strbuf.c_str(), "cannot assign custom vars to npc with a template");
			break;
		};
	}
}
BFF_API void BFF_ParseElement(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	bff_runtime_t* r = file->run;
	uint8_t context = file->context;
	switch (context) {
	case CONTEXT_NO_CHUNK:
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(), "cannot declare elements outside of a chunk");
		break;
	case CONTEXT_MAP_CHUNK:
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(), "map chunks currently don't support elements");
		break;
	case CONTEXT_DATA_CHUNK: {
		switch (file->c_data->dtype) {
		case BFF_DATA_ITEM:
			BFF_ParseItemElement(file);
			break;
		case BFF_DATA_WPN:
			BFF_ParseWpnElement(file);
			break;
		case BFF_DATA_MOB:
			BFF_ParseMobElement(file);
			break;
		case BFF_DATA_NPC:
			BFF_ParseNPCElement(file);
			break;
		};
		break; }
	};
}