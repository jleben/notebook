#include "document.h"
#include "filesystem.h"

namespace Notebook {

string Document::id_from_path(const string & path)
{
    return file_name_base(file_name(path));
}

}
