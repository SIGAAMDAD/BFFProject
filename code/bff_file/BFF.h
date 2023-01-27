#ifndef __BFFLANG__
#define __BFFLANG__

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include <err.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mman.h>

#define ASSERT(x, ...) \
{ \
	if (!(x)) { \
		fprintf(stderr, "%s(): ", __func__); \
		fprintf(stderr, __VA_ARGS__); \
		exit(-1); \
	} \
}
#define BFF_API extern "C"
#define BFF_INTERNAL extern "C"

typedef uint_fast8_t bffnum_t;
typedef uint_fast8_t byte;

#ifdef __cplusplus
extern "C" {
#endif

#define BFF_COMPILE       1
#define BFF_DECOMPILE     2
#define BFF_VERSION_MAJOR 0
#define BFF_VERSION_MINOR 1

typedef struct coord_s
{
	int16_t y;
	int16_t x;
} coord_t;

typedef struct _entity_spawner
{
	uint32_t entity_id;
	coord_t spawn_loc;
} entity_spawner_t;

#include "bff_types.h"
#include "bff_error.h"
#include "bff_map.h"
#include "bff_element/bff_mob.h"
#include "bff_element/bff_npc.h"
#include "bff_element/bff_item.h"
#include "bff_element/bff_wpn.h"
#include "bff_data.h"

#define LOG(...)                         \
{                                        \
	fprintf(stdout, "%s(): ", __func__); \
	fprintf(stdout, __VA_ARGS__);        \
	fputc('\n', stdout);                 \
}

typedef struct token_s
{
	char ftok;
	char btok;
	size_t line;
	std::string buffer;
} token_t;

enum
{
	CHUNK_DECLARE,
	PROP_DECLARE,
	LIST_OPEN,
	LIST_CLOSE,
	VAR_DECLARE,
	SPECIAL_VAR_DECLARE,
	DEFAULT_VALUE,
	CODE_OPEN,
	CODE_CLOSE,
	SUBCHUNK_OPEN,
	SUBCHUNK_CLOSE,
	NEWLINE,
	NULL_TOK,
	ELEMENT_DECLARE,
	DATA_OPEN,
	DATA_CLOSE,
	
	NUMTOKENS
};

#ifdef __cplusplus
}
#endif

static constexpr char tokens[NUMTOKENS] = {
	'#',
	'%',
	'[',
	']',
	'$',
	'&',
	':',
	'{',
	'}',
	'(',
	')',
	'\n',
	'\0',
	'@',
	'1',
	'0'
};

typedef struct _bff_runtime
{
	uint64_t i;
	std::vector<token_t> tokbuf;
	std::vector<std::string> strbuf;
} bff_runtime_t;

typedef struct _bff_file
{
	std::ifstream* file;
	bff_runtime_t* run;
	std::string filename;
	std::string output;
	const char* bffname;
	uint32_t numbiomes;
	uint32_t numwpns;
	uint32_t numitems;
	uint32_t nummobs;
	uint32_t numnpcs;
	uint32_t bfftype;
	uint8_t context;
	bffmap_t mchunk;
	bffdata_t* c_data;
	std::vector<bffdata_t> dchunks;
	std::vector<entity_spawner_t> spawners;
} bff_file_t;

BFF_API bff_file_t* BFF_InitFile(const char* filename);
BFF_API bff_file_t* BFF_Init(int argc, char** argv);
BFF_API void BFF_Free(bff_file_t* file);
BFF_API std::vector<bffdata_t>& BFF_GetDataChunks(bff_file_t* file);
BFF_API bffmap_t* BFF_GetMapChunk(bff_file_t* file);
BFF_API std::vector<bffwpn_t>& BFF_GetWpnElements(bff_file_t* file);
BFF_API std::vector<bffitem_t>& BFF_GetItemElements(bff_file_t* file);
BFF_API std::vector<bffmob_t>& BFF_GetMobElements(bff_file_t* file);
BFF_API void BFF_InitMapChunk(bffmap_t* mchunk);
BFF_API unsigned char BFF_GetFileType(bff_file_t* file);
BFF_API void BFF_Parse(bff_file_t* file);
BFF_API void BFF_Compile(bff_file_t* file, const char* outfile);
BFF_API void BFF_Decompile(bff_file_t* file, const char* infile);
BFF_API void BFF_ParseMapElement(bff_file_t* file);
BFF_API void BFF_ParseItemElement(bff_file_t* file);
BFF_API void BFF_ParseWpnElement(bff_file_t* file);
BFF_API void BFF_ParseMobElement(bff_file_t* file);
BFF_API void BFF_ParseNPCElement(bff_file_t* file);
BFF_API void BFF_ParseElement(bff_file_t* file);
BFF_API void BFF_ParseProperties(bff_file_t* file);
BFF_API void BFF_ParseChunkDec(bff_file_t* file);
BFF_API void BFF_ParseSubChunk(bff_file_t* file);
BFF_API void BFF_ParseMapSector(bff_file_t* file);
BFF_API void BFF_ParseDataSubChunk(bff_file_t* file);
BFF_API void BFF_GetNumU32(uint32_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNumU16(uint16_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNumU8(uint8_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNum32(int32_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNum16(int16_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNum8(int8_t* var, const std::string& str, size_t i);

BFF_INTERNAL void BFF_SystemErr(size_t err);
BFF_INTERNAL void BFF_ThrowError(size_t line, const char* buffer, const char* err, ...);

#endif
