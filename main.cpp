#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
	

typedef struct _bff_file bff_file_t;

typedef void (*bff_func)(bff_file_t*);
typedef bff_file_t* (*init_func)(const char*);

int main(int argc, char** argv)
{
	void* handle = dlopen("./libbffproject.so.0.1", RTLD_NOW | RTLD_LOCAL);
	if (!handle) { fprintf(stderr, "Error: %s\n", dlerror()); exit(-1); }
	
	init_func init;
	*(void **)(&init) = dlsym(handle, "BFF_InitFile");
	if (!init) { fprintf(stderr, "Error: %s\n", dlerror()); exit(-1); }
	
	bff_file_t* bff = (*init)(argv[1]);

	bff_func parse, free;
	*(void **)(&free) = dlsym(handle, "BFF_Free");
	*(void **)(&parse) = dlsym(handle, "BFF_Parse");
	(*parse)(bff);
	(*free)(bff);
	return 0;
}