#include "main_win.h"
#include "search_results_view.h"
#include "search.h"

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
    auto layout = new QVBoxLayout(this);

    d_query_input = new QLineEdit;
    d_query_input->setPlaceholderText("Search...");

    d_search_results = new Search_Result_View;

    layout->addWidget(d_query_input);
    layout->addWidget(d_search_results);

    connect(d_query_input, &QLineEdit::editingFinished,
            this, &Main_Window::search);
    connect(d_search_results, &QListWidget::itemActivated,
            this, &Main_Window::openCurrentSearchResult);
}

void Main_Window::openDatabase(const QString & path)
{
    try
    {
        d_db = Xapian::Database(path.toStdString());
    }
    catch (Xapian::Error & e)
    {
        QString msg;
        msg += "Failed to open database at\n";
        msg += path;
        msg += "\n\nReason: ";
        msg += QString::fromStdString(e.get_msg());

        QMessageBox::critical(this, "Open Database", msg);
    }
}

void Main_Window::search()
{
    // FIXME: Check if database open.

    if (d_query_input->text().isEmpty())
    {
        d_search_results->clear();
        return;
    }

    string query_source = d_query_input->text().toStdString();

    Xapian::QueryParser query_parser;
    query_parser.set_database(d_db);
    auto query = query_parser.parse_query(query_source);

    auto results = Notebook::search(d_db, query);

    d_search_results->set_search_result(results);
}

void Main_Window::openCurrentSearchResult()
{
    auto path = d_search_results->path(d_search_results->currentItem());

    qDebug() << path;

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}


}
