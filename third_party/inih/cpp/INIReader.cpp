// Read an INI file into easy-to-access name/value pairs.

// SPDX-License-Identifier: BSD-3-Clause

// Copyright (C) 2009-2020, Ben Hoyt

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include "../ini.h"
#include "INIReader.h"

using std::string;

INIReader::INIReader(const string& filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

INIReader::INIReader(const char *buffer, size_t buffer_size)
{
  string content(buffer, buffer_size);
  _error = ini_parse_string(content.c_str(), ValueHandler, this);
}

int INIReader::ParseError() const
{
    return _error;
}

string INIReader::Get(const string& section, const string& name, const string& default_value) const
{
    string key = MakeKey(section, name);
    // Use _values.find() here instead of _values.at() to support pre C++11 compilers
    return _values.count(key) ? _values.find(key)->second : default_value;
}

string INIReader::GetString(const string& section, const string& name, const string& default_value) const
{
    const string str = Get(section, name, "");
    return str.empty() ? default_value : str;
}

long INIReader::GetInteger(const string& section, const string& name, long default_value) const
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

INI_API int64_t INIReader::GetInteger64(const std::string& section, const std::string& name, int64_t default_value) const
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    int64_t n = strtoll(value, &end, 0);
    return end > value ? n : default_value;
}

unsigned long INIReader::GetUnsigned(const string& section, const string& name, unsigned long default_value) const
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    unsigned long n = strtoul(value, &end, 0);
    return end > value ? n : default_value;
}

INI_API uint64_t INIReader::GetUnsigned64(const std::string& section, const std::string& name, uint64_t default_value) const
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    uint64_t n = strtoull(value, &end, 0);
    return end > value ? n : default_value;
}

double INIReader::GetReal(const string& section, const string& name, double default_value) const
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

bool INIReader::GetBoolean(const string& section, const string& name, bool default_value) const
{
    string valstr = Get(section, name, "");
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(),
        [](const unsigned char& ch) { return static_cast<unsigned char>(::tolower(ch)); });
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

bool INIReader::HasSection(const string& section) const
{
    const string key = MakeKey(section, "");
    std::map<string, string>::const_iterator pos = _values.lower_bound(key);
    if (pos == _values.end())
        return false;
    // Does the key at the lower_bound pos start with "section"?
    return pos->first.compare(0, key.length(), key) == 0;
}

bool INIReader::HasValue(const string& section, const string& name) const
{
    string key = MakeKey(section, name);
    return _values.count(key);
}

string INIReader::MakeKey(const string& section, const string& name)
{
    string key = section + "=" + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(),
        [](const unsigned char& ch) { return static_cast<unsigned char>(::tolower(ch)); });
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    if (!name)  // Happens when INI_CALL_HANDLER_ON_NEW_SECTION enabled
        return 1;
    INIReader* reader = static_cast<INIReader*>(user);
    string key = MakeKey(section, name);
    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value ? value : "";
    return 1;
}
