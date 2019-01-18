#include "search_results_view.h"

namespace Notebook {

void Search_Result_View::set_search_result(const Search_Result & result)
{
    clear();

    for(auto & doc : result.documents())
    {
        QString title;
        if (!doc.title().empty())
            title = QString::fromStdString(doc.title());
        else
            title = QString::fromStdString(doc.path());

        auto item = new QListWidgetItem;
        item->setText(title);
        item->setData(Qt::UserRole, QString::fromStdString(doc.path()));

        addItem(item);
    }
}

QString Search_Result_View::path(QListWidgetItem* item)
{
    return item->data(Qt::UserRole).toString();
}

}
