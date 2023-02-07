#include "code/bff_file/BFF.h"
#include <exception>
int main(int argc, char** argv)
{
	bff_file_t* file = BFF_InitFile("nomadmain.bffl");
	BFF_Parse(file);
	BFF_Compile(file, "nomadmain.bffc");
	BFF_Decompile(file, "nomadmain.")
	return 0;
}