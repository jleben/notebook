#include "main_win.h"
#include "search_results_view.h"
#include "search.h"
#include "note_editor.h"

#include <xapian.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

#include <sstream>

using namespace std;

namespace Notebook {

Main_Window::Main_Window()
{
    auto layout = new QHBoxLayout(this);

    auto search_layout = new QVBoxLayout;

    d_query_input = new QLineEdit;
    d_query_input->setPlaceholderText("Search...");

    d_search_results = new Search_Result_View;

    d_note_editor = new Note_Editor;

    search_layout->addWidget(d_query_input);
    search_layout->addWidget(d_search_results);

    layout->addLayout(search_layout, 1);
    layout->addWidget(d_note_editor, 3);

    connect(d_query_input, &QLineEdit::editingFinished,
            this, &Main_Window::search);
    connect(d_search_results, &QListWidget::itemActivated,
            this, &Main_Window::openCurrentSearchResult);
}

void Main_Window::openDatabase(const QString & index_path, const QString & doc_path)
{
    try
    {
        d_db = new Database(index_path, doc_path, this);
    }
    catch (Xapian::Error & e)
    {
        QString msg;
        msg += "Failed to open database at\n";
        msg += index_path + " : " + doc_path;
        msg += "\n\nReason: ";
        msg += QString::fromStdString(e.get_msg());

        QMessageBox::critical(this, "Open Database", msg);

        return;
    }

    connect(d_db, &Database::searchFinished, this, &Main_Window::onSearchFinished);

    d_db->fullReindex();
}

void Main_Window::search()
{
    if (!d_db)
        return;

    d_db->search(d_query_input->text());
#if 0
    string query_source = d_query_input->text().toStdString();

    Xapian::QueryParser query_parser;
    query_parser.set_database(d_db);
    auto query = query_parser.parse_query(query_source);

    auto results = Notebook::search(d_db, query);

    d_search_results->set_search_result(results);
#endif
}

void Main_Window::onSearchFinished(Search_Result result)
{
    qDebug() << "Search finished.";

    d_search_results->set_search_result(result);
}

void Main_Window::openCurrentSearchResult()
{
    auto path = d_search_results->path(d_search_results->currentItem());

    qDebug() << path;

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}


}
