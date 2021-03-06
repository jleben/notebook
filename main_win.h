#pragma once

#include "database.h"

#include <QWidget>
#include <QLineEdit>
#include <QMainWindow>
#include <string>

namespace Notebook {

class Search_Result_View;
class Note_Editor;

class Main_Window : public QMainWindow
{
    Q_OBJECT

public:
    Main_Window();

    void openDatabase(const QString & index_path, const QString & doc_path);
    void openCurrentSearchResult();

public slots:
    void openDatabase();
    void saveCurrentDocumentAs();

private:
    void setupMenu();
    void search();
    void onSearchFinished(Search_Result result);

    Database * d_db = nullptr;
    QLineEdit * d_query_input = nullptr;
    Search_Result_View * d_search_results = nullptr;
    Note_Editor * d_note_editor = nullptr;
};

}
