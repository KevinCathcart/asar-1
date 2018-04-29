#include "platform/file-helpers.h"

//modified from somewhere in nall (license: ISC)
string dir(char const *name)
{
	string result = name;
	for (signed i = (int)strlen(result); i >= 0; i--)
	{
		if (result[i] == '/' || result[i] == '\\')
		{
			result[i + 1] = 0;
			break;
		}
		if (i == 0) result = "";
	}
	return result;
}

string create_combined_path(const char* path1, const char* path2)
{
	string combined = path1;

	int length = combined.truelen();

	if (combined.len > 0
		&& combined[length - 1] != '\\' && combined[length - 1] != '/')
	{
		combined += get_native_path_separator();
	}

	combined += path2;

	return normalize_path(combined);
}


// RPG Hacker: This function attempts to normalize a path.
// This means that it attempts to put it into a format
// where a simple string compare between two paths
// referring to the same file will hopefully always
// succeed.
// There are a few known problems and limitations:
// - Relative paths are not recommended, as they can't be
//   resolved reliably. For example, in this path
//   ../patch/main.asm
//   we can't collapse the .. because we don't know the
//   parent directory. A comparison will also fail when
//   one piece of code uses a relative path while another
//   piece of code uses an absolute path. For those
//   reasons, it's recommended to always use absolute paths
//   everywhere if possible.
// - Windows network paths like
//   \\MY_PC\patch\main.asm
//   Will likely break with this, since the function converts
//   all back slashes to forward slashes. I think Windows
//   actually requires back slashes for network paths.
// - Currently, the code doesn't look at drive letters
//   and just treats them as folders, so a path like
//   C:/../patch/main.asm
//   would result in
//   patch/main.asm
//   (However, a path like that is invalid, anyways, so
//   this hopefully doesn't matter).
string normalize_path(const char* path)
{
	string normalized = path;

	
	// RPG Hacker: Replace all \ in path by /
	// (Windows should work with / in paths just fine).
	// Note: will likely break network paths on Windows,
	// which still expect a prefix of \\, but does anyone
	// seriously even use them with Asar?
	for (int i = 0; i < normalized.truelen(); ++i)
	{
		if (normalized[i] == '\\')
		{
			normalized[i] = '/';
		}
	}

	// RPG Hacker: Collapse path by removing all unnecessary
	// . or .. path components.
	// As a little hack, just overwrite all the stuff we're
	// going to remove with 0x01 and remove it later. Probably
	// easier than to always shorten the path immediately.
	// Also using \x01 instead of \0 so that the path is still
	// easily readable in a debugger (since it normally just
	// assumes a string to end at \0). I don't think \x01 can
	// be used in valid paths, so this should be okay.
	char* previous_dir_start_pos = nullptr;
	char* current_dir_start_pos = normalized.str;
	while (*current_dir_start_pos == '/')
	{
		++current_dir_start_pos;
	}
	char* current_dir_end_pos = current_dir_start_pos;


	while (*current_dir_start_pos != '\0')
	{
		while (*current_dir_end_pos != '/' && *current_dir_end_pos != '\0')
		{
			++current_dir_end_pos;
		}

		size_t length = (size_t)(current_dir_end_pos - current_dir_start_pos);
		if (strncmp(current_dir_start_pos, ".", length) == 0)
		{
			// Found a . path component - remove it.
			while (current_dir_start_pos != current_dir_end_pos)
			{
				*current_dir_start_pos = '\x01';
				++current_dir_start_pos;
			}

			if (*current_dir_start_pos == '/')
			{
				*current_dir_start_pos = '\x01';
				++current_dir_end_pos;
			}
		}
		else if (strncmp(current_dir_start_pos, "..", length) == 0)
		{
			// Found a .. path component - if we have any known
			// folder before it, remove them both.
			if (previous_dir_start_pos != nullptr)
			{
				// previous_dir_start_pos and current_dir_start_pos are immediately
				// set to something else shortly after, so it should be
				// okay to move them directly here and not use a
				// temporary variable.
				while (*previous_dir_start_pos != '/' && *previous_dir_start_pos != '\0')
				{
					*previous_dir_start_pos = '\x01';
					++previous_dir_start_pos;
				}

				if (*previous_dir_start_pos == '/')
				{
					*previous_dir_start_pos = '\x01';
				}

				while (current_dir_start_pos != current_dir_end_pos)
				{
					*current_dir_start_pos = '\x01';
					++current_dir_start_pos;
				}

				if (*current_dir_start_pos == '/')
				{
					*current_dir_start_pos = '\x01';
					++current_dir_end_pos;
				}
			}

			previous_dir_start_pos = nullptr;
		}
		else
		{
			if (*current_dir_end_pos == '/')
			{
				++current_dir_end_pos;
			}

			previous_dir_start_pos = current_dir_start_pos;
		}

		current_dir_start_pos = current_dir_end_pos;
	}


	// Now construct our new string by copying everything but the \x01 into it.
	string copy;

	for (int i = 0; i < normalized.truelen(); ++i)
	{
		if (normalized[i] != '\x01')
		{
			copy += normalized[i];
		}
	}

	normalized = copy;


	// RPG Hacker: On Windows, file paths are case-insensitive.
	// It isn't really good style to mix different cases, especially
	// when referring to the same file, but it's theoretically possible
	// to do so, so we need to handle it and not have Asar fail in this case.
	// The easiest way to handle it is to convert the entire path to lowercase.
#if defined(windows)
	normalized = lower(normalized);
#endif


	// TODO: Maybe resolve symbolic links here?
	// Not sure if it's a good idea and if it's needed in the first place.
	// In theory, it could lead to some problems with files that exist
	// only in memory.

	return normalized;
}
