#include "BFF.h"

BFF_API void signal_seggy(int signum)
{
	LOG("caught a segmentation fault");
	exit(-1);
}

BFF_API void BFF_InitMapChunk(bffmap_t* mchunk)
{
	mchunk->sectorcounter = -1;
	memset(&mchunk->sectormap, '.', sizeof(mchunk->sectormap));
}

// to be used by the game to get data from .bffl files
BFF_API bff_file_t* BFF_InitFile(const char* filename)
{
	bff_file_t* bff = (bff_file_t *)malloc(sizeof(bff_file_t));
	ASSERT(bff, "Error: failed to malloc() bff_file_t struct!\n");
	bff->bfftype = BFF_UNDEFINED;
	bff->numbiomes = BFF_UNDEFINED;
	bff->numitems = BFF_UNDEFINED;
	bff->numwpns = BFF_UNDEFINED;
	bff->nummobs = BFF_UNDEFINED;
	bff->numnpcs = BFF_UNDEFINED;
	bff->context = CONTEXT_NO_CHUNK;
	bff->filename = filename;
	bff->run = (bff_runtime_t *)malloc(sizeof(bff_runtime_t));
	ASSERT(bff->run, "Error: failed to malloc() bff_runtime_t struct!\n");
	std::string line;
	std::ifstream f(bff->filename.c_str(), std::ios::in);
	while (std::getline(f, line)) { bff->run->strbuf.push_back(line); }
	return bff;
}

BFF_API bff_file_t* BFF_Init(int argc, char** argv)
{
	if (argc < 2) {
		fprintf(stdout,
			"Usage: ./bff <input file> [options]\n"
			"\toptions when using this are:\n"
			"\t -o --output <file>     compile a .bffl into a .bffc (compiled bff data)\n"
			"\t --with-nc              enable nomadc extensions (disabled by default)\n"
			"\t -w --with-warnings     turn on warnings (disabled by default)\n"
			"\t -d --data              print the bff's formatted data to the console (disabled by default)\n"
		);
		exit(1);
	}
	bff_file_t* bff = (bff_file_t *)malloc(sizeof(bff_file_t));
	ASSERT(bff, "Error: failed to malloc() bff_file_t struct!\n");
	bff->bfftype = BFF_UNDEFINED;
	bff->context = CONTEXT_NO_CHUNK;
	bff->numbiomes = BFF_UNDEFINED;
	bff->numwpns = BFF_UNDEFINED;
	bff->numitems = BFF_UNDEFINED;
	bff->nummobs = BFF_UNDEFINED;
	bff->numnpcs = BFF_UNDEFINED;
	signal(SIGSEGV, signal_seggy);
	bff->filename = argv[1];
//	bool warnings = false;
//	bool dataout = false;
//	bool nc_extend = false;
	for (int32_t i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-o")
		|| strcmp(argv[i], "--output")) {
			bff->output = argv[i+1];
		}
		else if (strcmp(argv[i], "--with-nc")) {
//			nc_extend = true;
		}
		else if (strcmp(argv[i], "-w")
		|| strcmp(argv[i], "--with-warnings")) {
//			warnings = true;
		}
		else if (strcmp(argv[i], "-d")
		|| strcmp(argv[i], "--data")) {
//			dataout = true;
		}
	}
	return bff;
}

BFF_API unsigned char BFF_GetFileType(bff_file_t* file)
{
	if (file->filename.find(".bffc") != NotFound) {
		return BFF_COMPILE;
	} else if (file->filename.find(".bffl") != NotFound) {
		return BFF_DECOMPILE;
	}
	return BFF_DECOMPILE;
}

BFF_API void BFF_GetNumU32(uint32_t* var, const std::string& strbuf, size_t i)
{
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("MAX_ALLOWED") != NotFound)
			*var = BFF_MAX_ALLOWED;
		else
			BFF_ThrowError(i, strbuf.c_str(), "invalid default value for variable");
	} else {
		*var = (uint32_t)stoi(strbuf);
	}
}

BFF_API void BFF_GetNumU16(uint16_t* var, const std::string& strbuf, size_t i)
{
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("MAX_ALLOWED") != NotFound)
			*var = BFF_MAX_ALLOWED;
		else
			BFF_ThrowError(i, strbuf.c_str(), "invalid default value for variable");
	} else {
		*var = (uint16_t)stoi(strbuf);
	}
}
BFF_API void BFF_GetNumU8(uint8_t* var, const std::string& strbuf, size_t i)
{
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("MAX_ALLOWED") != NotFound)
			*var = BFF_MAX_ALLOWED;
		else
			BFF_ThrowError(i, strbuf.c_str(), "invalid default value for variable");
	} else {
		*var = (uint8_t)stoi(strbuf);
	}
}
BFF_API void BFF_GetNum32(int32_t* var, const std::string& strbuf, size_t i)
{
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("MAX_ALLOWED") != NotFound)
			*var = BFF_MAX_ALLOWED;
		else
			BFF_ThrowError(i, strbuf.c_str(), "invalid default value for variable");
	} else {
		*var = (int32_t)stoi(strbuf);
	}
}
BFF_API void BFF_GetNum16(int16_t* var, const std::string& strbuf, size_t i)
{
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("MAX_ALLOWED") != NotFound)
			*var = BFF_MAX_ALLOWED;
		else
			BFF_ThrowError(i, strbuf.c_str(), "invalid default value for variable");
	} else {
		*var = (int16_t)stoi(strbuf);
	}
}
BFF_API void BFF_GetNum8(int8_t* var, const std::string& strbuf, size_t i)
{
	if (strbuf.front() == tokens[DEFAULT_VALUE]) {
		if (strbuf.find("MAX_ALLOWED") != NotFound)
			*var = BFF_MAX_ALLOWED;
		else
			BFF_ThrowError(i, strbuf.c_str(), "invalid default value for variable");
	} else {
		*var = (int8_t)stoi(strbuf);
	}
}


BFF_API void BFF_SystemErr(size_t err)
{
	char buffer[256];
	switch (err) {
	case ERR_INVALID_CONTEXT:
		snprintf(buffer, sizeof(buffer), "Unknown/Invalid Context");
		break;
	};
	fprintf(stderr, "Internal Error: %s\n", buffer);
	fflush(stderr);
	exit(-1);
}

BFF_API void BFF_ThrowError(size_t line, const char* buffer, const char* err, ...)
{
	fprintf(stderr, "(ERROR)\nAt Line: %lu\nContents: %s\nMessage: ", line, buffer);
	va_list argptr;
	va_start(argptr, err);
	vfprintf(stderr, err, argptr);
	va_end(argptr);
	fprintf(stderr, "\n");
	fflush(stderr);
	exit(-1);
}