#include "database.h"

#include <QDebug>
#include <QCoreApplication>

namespace Notebook {

Database::Database(const QString & index_path, const QString & docs_path):
    d_index_path(index_path),
    d_docs_path(docs_path)
{
    // FIXME: Exception handling:
    d_worker = new Database_Worker(this);

    d_worker->moveToThread(&d_work_thread);

    connect(d_worker, &Database_Worker::searchFinished,
            this, &Database::searchFinished);

    d_work_thread.start();
}

Database::~Database()
{
    d_work_thread.quit();
    d_work_thread.wait();
    delete d_worker;
}

void Database::search(const QString & query)
{
    qDebug() << "Posting search request";

    auto request = new Database_Worker::Search_Request(query);

    QCoreApplication::removePostedEvents(d_worker, Database_Worker::Search_Request_Event);
    QCoreApplication::postEvent(d_worker, request);
}

Database_Worker::Database_Worker(Database * db):
    d_db(db)
{
    // FIXME: Exception handling
    d_xdb = Xapian::Database(db->index_path().toStdString());
}

void Database_Worker::customEvent(QEvent * event)
{
    switch(event->type())
    {
    case Search_Request_Event:
    {
        auto e = static_cast<Search_Request*>(event);
        search(*e);
        break;
    }
    default:
        break;
    }
}

void Database_Worker::search(const Search_Request & request)
{
    qDebug() << "Processing search request.";

    Xapian::QueryParser query_parser;
    query_parser.set_database(d_xdb);
    auto query = query_parser.parse_query(request.query_text.toStdString());

    auto results = Notebook::search(d_xdb, query);

    emit searchFinished(results);
}


}
