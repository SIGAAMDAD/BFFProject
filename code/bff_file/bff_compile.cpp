#include "BFF.h"

#ifdef __unix__
typedef int32_t bffnum_t;
#elif defined(_WIN32)
typedef INT32 bffnum_t;
#endif

#define BUFFER_SIZE 10*1024*1024
#define BFFHEADER 0xa49d21f
#define CHUNK_BEGIN 0x0c1
#define CHUNK_END 0x0c2
#define BFF_TYPE_CAMPAIGN 0xff1
#define BFF_TYPE_ROGUELIKE 0xff2
#define BFF_TYPE_DOOMCLONE 0xff3

typedef struct
{
	bffnum_t magic = (bffnum_t)BFFHEADER;
	bool root = false;
	bool has_items = false;
	bool has_wpns = false;
	bool has_mobs = false;
	bool has_npcs = false;
	bffnum_t bfftype;
	bffnum_t total_chunks;
	char bffname[256];
} BFFheader;

enum : bffnum_t {CHUNK_TYPE_MAP, CHUNK_TYPE_ITEM, CHUNK_TYPE_WPN, CHUNK_TYPE_MOB, CHUNK_TYPE_NPC, CHUNK_TYPE_AUDIO};

typedef struct
{
	bffnum_t chunk_bheader = CHUNK_BEGIN;
	bffnum_t chunktype;
	char buffer[BUFFER_SIZE];
	bffnum_t chunk_eheader = CHUNK_END;
} BFFchunk;

static FILE* fp;
#define NUM_CHUNKS (sizeof(chunks)/sizeof(chunks[0]))

BFF_API void BFF_Decompile(bff_file_t* file, const char* infile)
{
	BFFheader header;
	fp = fopen(infile, "rb");
	if (!fp) exit(-1);
	fread(&header, sizeof(BFFheader), 1, fp);
	if (!(header.magic & BFFHEADER)) {
		fprintf(stderr, "Error: .bffc header incorrect magic number!\n");
		exit(-1);
	}
	BFFchunk* chunks = (BFFchunk *)malloc(sizeof(BFFchunk) * header.total_chunks);
	fread(&chunks[0], sizeof(BFFchunk), 1, fp);
	switch (chunks[0].chunktype) {
	case CHUNK_TYPE_MAP: printf("Map chunk found\n");
	};
	fclose(fp);
}

BFF_API void BFF_Compile(bff_file_t* file, const char* outfile)
{
	BFFheader header;
	memset(header.bffname, '\0', sizeof(header.bffname));
	strncpy(header.bffname, file->bffname, strlen(file->bffname));
	header.total_chunks = file->dchunks.size() + 1;
	BFFchunk* chunks = (BFFchunk *)malloc(sizeof(BFFchunk) * header.total_chunks);
	header.bfftype = file->bfftype;
	for (bffnum_t i = 0; i < file->dchunks.size(); ++i) {
		if (file->dchunks[i].dtype == BFF_DATA_ITEM) header.has_items = true;
		else if (file->dchunks[i].dtype == BFF_DATA_WPN) header.has_wpns = true;
		else if (file->dchunks[i].dtype == BFF_DATA_MOB) header.has_mobs = true;
		else if (file->dchunks[i].dtype == BFF_DATA_NPC) header.has_npcs = true;
	}
	fp = fopen(outfile, "wb");
	ASSERT(fp, "Could not open output .bffc file!");
	fwrite(&header, sizeof(BFFheader), 1, fp);
	BFFchunk* const mapchunk = &chunks[0];
	mapchunk->chunktype = CHUNK_TYPE_MAP;
	fwrite(&mapchunk->chunk_bheader, sizeof(bffnum_t), 1, fp);
	fwrite(&mapchunk->chunktype, sizeof(bffnum_t), 1, fp);
	fwrite(&file->mchunk.sectormap, sizeof(file->mchunk.sectormap), sizeof(char), fp);
	fwrite(&mapchunk->chunk_eheader, sizeof(bffnum_t), 1, fp);
	fclose(fp);
}