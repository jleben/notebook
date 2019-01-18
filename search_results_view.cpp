#include "search_results_view.h"

namespace Notebook {

void Search_Result_View::set_search_result(const Search_Result & result)
{
    clear();

    for(auto & doc : result.documents())
    {
        if (!doc.title().empty())
            addItem(QString::fromStdString(doc.title()));
        else
            addItem(QString::fromStdString(doc.path()));
    }
}

}
