#if defined(_WIN32)
#	define NOMINMAX

#	if defined(_MSC_VER)
#		pragma warning(push)
#		pragma warning(disable : 4365)
#		pragma warning(disable : 4571)
#		pragma warning(disable : 4623)
#		pragma warning(disable : 4625)
#		pragma warning(disable : 4626)
#		pragma warning(disable : 4668)
#		pragma warning(disable : 4711)
#		pragma warning(disable : 4774)
#		pragma warning(disable : 4987)
#		pragma warning(disable : 5026)
#		pragma warning(disable : 5027)
#	endif

#	include <stdio.h>
#	include <string.h>
#	include <stdlib.h>
#	include <sys/stat.h>
#	include <windows.h>
#	include <vector>
#	include <string>
#	include <algorithm>
#	include <set>

#	if defined(_MSC_VER)
#		pragma warning(pop)
#	endif
#else
#	include <dirent.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <vector>
#	include <string>
#	include <string.h>
#	include <algorithm>
#	include <sys/stat.h>
#	include <set>
#endif


#define min(a, b) ((a)<(b)?(a):(b))

static const size_t max_rom_size = 16777216u;

inline int file_exist(const char *filename)
{
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

std::string dir(char const *name);

inline bool str_ends_with(const char * str, const char * suffix)
{
	if (str == NULL || suffix == NULL)
		return false;

	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);

	if (suffix_len > str_len)
		return false;

	return (strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0);
}


struct wrapped_file
{
	char file_name[64];
	char file_path[512];
};

bool find_files_in_directory(std::vector<wrapped_file>& out_array, const char * directory_name);

void delete_file(const char * filename);

std::vector<std::string> tokenize_string(const char * str, const char * key);
