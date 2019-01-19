#pragma once

#include <string>

namespace Notebook {

using std::string;

class Document
{
public:
    enum Field
    {
        Path = 0,
        Title = 1,
        Date = 2,
    };

    static string id_from_path(const string & path);
};

}
