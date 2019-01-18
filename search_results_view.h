#pragma once

#include "search.h"

#include <QWidget>
#include <QListWidget>

namespace Notebook {

class Search_Result_View : public QListWidget
{
public:
    void set_search_result(const Search_Result &);
    QString path(QListWidgetItem*);
};

}
