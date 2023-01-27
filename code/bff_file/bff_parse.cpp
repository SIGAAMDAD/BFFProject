#include "BFF.h"

static void BFF_GetTokens(bff_file_t* file, bff_runtime_t* runtime)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	size_t line = 0;
	for (size_t i = 0; i < runtime->strbuf.size(); ++i) {
		runtime->tokbuf.emplace_back();
		token_t* const tok = &runtime->tokbuf.back();
		tok->ftok = runtime->strbuf[i].front();
		tok->btok = runtime->strbuf[i].back();
		tok->line = line;
		tok->buffer = runtime->strbuf[i];
		++line;
	}
}

BFF_API void BFF_Parse(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	file->context = CONTEXT_NO_CHUNK;
	LOG("successfully opened bff file, parsing");
	BFF_GetTokens(file, file->run);
	LOG("fetched bff tokens");
	file->context = CONTEXT_NO_CHUNK;
	for (file->run->i = 0; file->run->i < file->run->tokbuf.size(); ++file->run->i) {
		if (file->run->i == (file->run->tokbuf.size() - 1)) { break; }
		switch (file->run->tokbuf[file->run->i].ftok) {
		case tokens[NULL_TOK]:
		case tokens[NEWLINE]:
			break;
		case tokens[PROP_DECLARE]:
			BFF_ParseProperties(file);
			break;
		case tokens[SUBCHUNK_OPEN]:
			BFF_ParseSubChunk(file);
			break;
		case tokens[ELEMENT_DECLARE]:
			BFF_ParseElement(file);
			break;
		case tokens[CHUNK_DECLARE]:
			BFF_ParseChunkDec(file);
			break;
		};
	}
}

BFF_API void BFF_ParseSubChunk(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	bff_runtime_t* r = file->run;
	LOG("found a subchunk at line: %lu", r->i);
	switch (file->context) {
	case CONTEXT_NO_CHUNK:
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(), "cannot declare subchunk outside of a context");
		break;
	case CONTEXT_MAP_CHUNK:
		BFF_ParseMapSector(file);
		break;
	case CONTEXT_DATA_CHUNK:
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(), "subchunks within data chunks must be declared within elements");
	default:
		BFF_SystemErr(ERR_INVALID_CONTEXT);
		break;
	};
}

BFF_API void BFF_ParseMapSector(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	LOG("subchunk type: mapsector");
	file->mchunk.sectorcounter++;
	++file->run->i;
	uint16_t y{};
	for (y = 0; y < 120; ++y) {
		for (uint16_t x = 0; x < 120; ++x) {
			file->mchunk.sectormap[file->mchunk.sectorcounter][y][x] = file->run->tokbuf[file->run->i+y].buffer[x];
		}
	}
	file->run->i += y;
}

static uint32_t BFF_GetBFFType(bff_file_t* file, const std::string& strbuf)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("CAMPAIGN") != NotFound)
			file->bfftype = BFF_TYPE_CAMPAIGN;
		else if (strbuf.find("ROGUELIKE") != NotFound)
			file->bfftype = BFF_TYPE_ROGUELIKE;
		else if (strbuf.find("DOOMCLONE") != NotFound)
			file->bfftype = BFF_TYPE_DOOMCLONE;
		else
			BFF_ThrowError(file->run->i, strbuf.c_str(), "invalid bff type: %s", strbuf.c_str());
	} else {
		BFF_ThrowError(file->run->i, strbuf.c_str(), "invalid bff type: %s", strbuf.c_str());
	}
	return file->bfftype;
}

BFF_API void BFF_ParseProperties(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	LOG("found bff properties");
	bff_runtime_t *r = file->run;
	for (;; ++r->i) {
		if (r->tokbuf[r->i].ftok == tokens[PROP_DECLARE]
		&& r->tokbuf[r->i].btok == tokens[DATA_CLOSE]) { break; }
		const std::string strbuf = r->tokbuf[r->i].buffer;
		if (r->tokbuf[r->i].ftok == tokens[VAR_DECLARE]) {
			if (strbuf.find("NAME") != NotFound) {
				if (file->context == CONTEXT_NO_CHUNK)
					file->bffname = r->tokbuf[r->i+1].buffer.c_str();
				else if (file->context == CONTEXT_MAP_CHUNK)
					file->mchunk.mapname = r->tokbuf[r->i+1].buffer;
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			} else if (strbuf.find("TYPE") != NotFound) {
				if (file->context == CONTEXT_NO_CHUNK)
					file->bfftype = BFF_GetBFFType(file, r->tokbuf[r->i+1].buffer);
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			} else if (strbuf.find("NUMITEMS") != NotFound) {
				if (file->context == CONTEXT_NO_CHUNK)
					BFF_GetNumU32(&file->numitems, r->tokbuf[r->i+1].buffer, (r->i+1));
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			} else if (strbuf.find("NUMWPNS") != NotFound) {
				if (file->context == CONTEXT_NO_CHUNK)
					BFF_GetNumU32(&file->numwpns, r->tokbuf[r->i+1].buffer, (r->i+1));
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "inavlid property var");
			} else if (strbuf.find("NUMMOBS") != NotFound) {
				if (file->context == CONTEXT_NO_CHUNK)
					BFF_GetNumU32(&file->nummobs, r->tokbuf[r->i+1].buffer, (r->i+1));
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			} else if (strbuf.find("NUMNPCS") != NotFound) {
				if (file->context == CONTEXT_NO_CHUNK)
					BFF_GetNumU32(&file->numnpcs, r->tokbuf[r->i+1].buffer, (r->i+1));
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			}
		}
	}
}
BFF_API void BFF_ParseChunkDec(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!\n");
	bff_runtime_t* r = file->run;
	size_t i = r->i;
	LOG("found a chunk declaration at line: %lu", i);
	bool opening = r->tokbuf[i].btok == tokens[DATA_OPEN];
	bool closing = r->tokbuf[i].btok == tokens[DATA_CLOSE];
	if (!opening && !closing)
		BFF_ThrowError(i, r->tokbuf[i].buffer.c_str(), "invalid chunk syntax");
	const std::string& strbuf = r->tokbuf[i].buffer;
	if (strbuf.find("MAP") != NotFound) {
		switch (file->context) {
		case CONTEXT_NO_CHUNK:
			if (opening && !closing) {
				file->context = CONTEXT_MAP_CHUNK;
				BFF_InitMapChunk(&file->mchunk);
			}
			else if (!opening && closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot close map chunk without opening one");
			break;
		case CONTEXT_MAP_CHUNK:
			if (opening && !closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot open map chunk within another one");
			else if (!opening && closing)
				file->context = CONTEXT_NO_CHUNK;
			break;
		case CONTEXT_DATA_CHUNK:
			BFF_ThrowError(i, strbuf.c_str(), "cannot open or close data chunks within map chunks");
			break;
		default:
			BFF_SystemErr(ERR_INVALID_CONTEXT);
			break;
		};
	}
	else if (strbuf.find("DATA") != NotFound) {
		switch (file->context) {
		case CONTEXT_NO_CHUNK:
			if (opening && !closing) {
				file->context = CONTEXT_DATA_CHUNK;
				file->dchunks.emplace_back();
				file->c_data = &file->dchunks.back();
			}
			else if (!opening && closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot close data chunk without opening one");
			break;
		case CONTEXT_MAP_CHUNK:
			BFF_ThrowError(i, strbuf.c_str(), "cannot open or close map chunks within data chunks");
			break;
		case CONTEXT_DATA_CHUNK:
			if (opening && !closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot open data chunk within another one");
			else if (!opening && closing)
				file->context = CONTEXT_NO_CHUNK;
			break;
		default:
			BFF_SystemErr(ERR_INVALID_CONTEXT);
			break;
		};
	}
	else
		BFF_ThrowError(i, strbuf.c_str(), "invalid chunk type/declaration");
}