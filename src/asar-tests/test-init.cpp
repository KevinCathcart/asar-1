#include "util.hpp"
#include "asardll.h"


int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		// don't treat this as an error and just return 0
		printf("Usage: test-???.exe [path_to_tests_directory]\n");

		return 0;
	}

	if (!asar_init())
	{
		printf("Error: Failed to load Asar DLL.\n");
		return 1;
	}

	asar_close();

	if (!asar_init())
	{
		printf("Error: Failed to re-initialize Asar.\n");
		return 1;
	}

	asar_close();
	return 0;
}
