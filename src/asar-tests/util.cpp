#include "util.hpp"

// This function is based in part on nall, which is under the following licence.
// This modified version is licenced under the LGPL version 3 or later. See the LICENSE file
// for details.
//
//   Copyright (c) 2006-2015  byuu
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all
// copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
std::string dir(char const *name)
{
	std::string result = name;
	for (signed i = (int)result.length(); i >= 0; i--)
	{
		if (result[(size_t)i] == '/' || result[(size_t)i] == '\\')
		{
			result.erase((size_t)(i + 1));
			break;
		}
		if (i == 0) result = "";
	}
	return result;
}

bool find_files_in_directory(std::vector<wrapped_file>& out_array, const char * directory_name)
{
#if defined(_WIN32)

	char search_path[512];
	bool has_path_seperator = false;
	if (str_ends_with(directory_name, "/") || str_ends_with(directory_name, "\\"))
	{
		snprintf(search_path, sizeof(search_path), "%s*.*", directory_name);
		has_path_seperator = true;
	}
	else
	{
		snprintf(search_path, sizeof(search_path), "%s/*.*", directory_name);
	}

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (str_ends_with(fd.cFileName, ".asm"))
				{
					wrapped_file new_file;
					strncpy(new_file.file_name, fd.cFileName, sizeof(new_file.file_name));
					new_file.file_name[sizeof(new_file.file_name) - 1] = '\0';
					strncpy(new_file.file_path, directory_name, sizeof(new_file.file_path));
					if (!has_path_seperator)
					{
						strncat(new_file.file_path, "/", sizeof(new_file.file_path));
					}
					strncat(new_file.file_path, fd.cFileName, sizeof(new_file.file_path));
					new_file.file_path[sizeof(new_file.file_path) - 1] = '\0';
					out_array.push_back(new_file);
				}
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}
	else
	{
		return false;
	}

#else

	bool has_path_seperator = false;
	if (str_ends_with(directory_name, "/") || str_ends_with(directory_name, "\\"))
	{
		has_path_seperator = true;
	}
	DIR * dir;
	dirent * ent;
	if ((dir = opendir(directory_name)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			// Only consider regular files
			if (ent->d_type == DT_REG)
			{
				if (str_ends_with(ent->d_name, ".asm"))
				{
					wrapped_file new_file;
					strncpy(new_file.file_name, ent->d_name, sizeof(new_file.file_name));
					new_file.file_name[sizeof(new_file.file_name) - 1] = '\0';
					strncpy(new_file.file_path, directory_name, sizeof(new_file.file_path));
					if (!has_path_seperator)
					{
						strncat(new_file.file_path, "/", sizeof(new_file.file_path));
					}
					strncat(new_file.file_path, ent->d_name, sizeof(new_file.file_path));
					new_file.file_path[sizeof(new_file.file_path) - 1] = '\0';
					out_array.push_back(new_file);
				}
			}
		}
		closedir(dir);
	}
	else {
		return false;
	}

#endif

	return true;
}

void delete_file(const char * filename)
{
#if defined(_WIN32)

	DeleteFileA(filename);

#else

	remove(filename);

#endif
}

std::vector<std::string> tokenize_string(const char * str, const char * key)
{
	std::string s = str;
	std::string delimiter = key;

	size_t pos = 0;
	std::string token;
	std::vector<std::string> list;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		// Don't bother adding empty tokens (they're just whitespace)
		if (token != "")
		{
			list.push_back(token);
		}
		s.erase(0, pos + delimiter.length());
	}
	list.push_back(s);
	return list;
}
