#pragma once

#include <xapian.h>
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

private:
    void search();

    Xapian::Database d_db;
    QLineEdit * d_query_input;
    Search_Result_View * d_search_results;
};

}
