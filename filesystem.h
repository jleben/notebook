#pragma once

#include <string>

using std::string;

inline
string file_name(const string & path)
{
    string name = path;

    {
        auto pos = name.rfind('/');
        if (pos != string::npos)
            name = name.substr(pos+1);
    }

    return name;
}

inline
string file_name_base(const string & file_name)
{
    {
        auto pos = file_name.find('.');
        if (pos != string::npos)
            return file_name.substr(0, pos);
    }

    return file_name;
}

inline
string file_name_extension(const string & file_name)
{
    {
        auto pos = file_name.rfind('.');
        if (pos != string::npos)
            return file_name.substr(pos+1);
    }

    return string();
}
