#pragma once

#include "database.h"

#include <QWidget>
#include <QLineEdit>
#include <string>

namespace Notebook {

class Search_Result_View;

class Main_Window : public QWidget
{
    Q_OBJECT

public:
    Main_Window();

    void openDatabase(const QString & path);
    void openCurrentSearchResult();

private:
    void search();
    void onSearchFinished(Search_Result result);

    Database * d_db = nullptr;
    QLineEdit * d_query_input;
    Search_Result_View * d_search_results;
};

}
