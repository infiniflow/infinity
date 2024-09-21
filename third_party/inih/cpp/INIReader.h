// Read an INI file into easy-to-access name/value pairs.

// SPDX-License-Identifier: BSD-3-Clause

// Copyright (C) 2009-2020, Ben Hoyt

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#ifndef INIREADER_H
#define INIREADER_H

#include <map>
#include <string>
#include <cstdint>

// Visibility symbols, required for Windows DLLs
#ifndef INI_API
#if defined _WIN32 || defined __CYGWIN__
#	ifdef INI_SHARED_LIB
#		ifdef INI_SHARED_LIB_BUILDING
#			define INI_API __declspec(dllexport)
#		else
#			define INI_API __declspec(dllimport)
#		endif
#	else
#		define INI_API
#	endif
#else
#	if defined(__GNUC__) && __GNUC__ >= 4
#		define INI_API __attribute__ ((visibility ("default")))
#	else
#		define INI_API
#	endif
#endif
#endif

// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class INIReader
{
public:
    // Construct INIReader and parse given filename. See ini.h for more info
    // about the parsing.
    INI_API explicit INIReader(const std::string& filename);

    // Construct INIReader and parse given buffer. See ini.h for more info
    // about the parsing.
    INI_API explicit INIReader(const char *buffer, size_t buffer_size);

    // Return the result of ini_parse(), i.e., 0 on success, line number of
    // first error on parse error, or -1 on file open error.
    INI_API int ParseError() const;

    // Get a string value from INI file, returning default_value if not found.
    INI_API std::string Get(const std::string& section, const std::string& name,
                    const std::string& default_value) const;

    // Get a string value from INI file, returning default_value if not found,
    // empty, or contains only whitespace.
    INI_API std::string GetString(const std::string& section, const std::string& name,
                    const std::string& default_value) const;

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
    INI_API long GetInteger(const std::string& section, const std::string& name, long default_value) const;

    // Get a 64-bit integer (int64_t) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
    INI_API int64_t GetInteger64(const std::string& section, const std::string& name, int64_t default_value) const;

    // Get an unsigned integer (unsigned long) value from INI file, returning default_value if
    // not found or not a valid unsigned integer (decimal "1234", or hex "0x4d2").
    INI_API unsigned long GetUnsigned(const std::string& section, const std::string& name, unsigned long default_value) const;

    // Get an unsigned 64-bit integer (uint64_t) value from INI file, returning default_value if
    // not found or not a valid unsigned integer (decimal "1234", or hex "0x4d2").
    INI_API uint64_t GetUnsigned64(const std::string& section, const std::string& name, uint64_t default_value) const;

    // Get a real (floating point double) value from INI file, returning
    // default_value if not found or not a valid floating point value
    // according to strtod().
    INI_API double GetReal(const std::string& section, const std::string& name, double default_value) const;

    // Get a boolean value from INI file, returning default_value if not found or if
    // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
    // and valid false values are "false", "no", "off", "0" (not case sensitive).
    INI_API bool GetBoolean(const std::string& section, const std::string& name, bool default_value) const;

    // Return true if the given section exists (section must contain at least
    // one name=value pair).
    INI_API bool HasSection(const std::string& section) const;

    // Return true if a value exists with the given section and field names.
    INI_API bool HasValue(const std::string& section, const std::string& name) const;

private:
    int _error;
    std::map<std::string, std::string> _values;
    static std::string MakeKey(const std::string& section, const std::string& name);
    static int ValueHandler(void* user, const char* section, const char* name,
                            const char* value);
};

#endif  // INIREADER_H
