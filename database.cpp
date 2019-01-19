#include "database.h"
#include "document.h"
#include "index_markdown.h"

#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QDateTime>

namespace Notebook {

Database::Database(const QString & index_path, const QString & docs_path, QObject * parent):
    QObject(parent),
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

void Database::fullReindex()
{
    auto request = new Database_Worker::Full_Index_Request;
    QCoreApplication::removePostedEvents(d_worker, Database_Worker::Full_Index_Request_Event);
    QCoreApplication::postEvent(d_worker, request);
}


Database_Worker::Database_Worker(Database * db):
    d_db(db)
{
    // FIXME: Exception handling
    d_xdb = Xapian::WritableDatabase(db->index_path().toStdString());
}

Database_Worker::~Database_Worker()
{
    qDebug() << "Committing...";
    d_xdb.commit();
    qDebug() << "Done comitting.";
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
    case Full_Index_Request_Event:
    {
        fullReindex();
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

void Database_Worker::fullReindex()
{
    qDebug() << "Full reindex...";

    // FIXME: Remove documents without path or with duplicate path.
    // FIXME: Remove documents with duplicate ID terms, or ID not matching path.

    std::unordered_map<string, int64_t> indexedPaths;

    {
        auto path_it = d_xdb.valuestream_begin(Document::Path);
        auto paths_end = d_xdb.valuestream_end(Document::Path);
        for(; path_it != paths_end; ++path_it)
        {
            auto doc = d_xdb.get_document(path_it.get_docid());
            string path = *path_it;

            string date_text = doc.get_value(Document::Date);
            int64_t date;

            try { date = stoll(date_text); }
            catch (std::invalid_argument &) { date = 0; }

            qDebug() << "Found doc:"
                     << date_text.c_str() << "=" << date
                     << "at" << path.c_str();

            indexedPaths.emplace(path, date);
        }
    }

    {
        QDir dir(d_db->docs_path());

        auto file_infos = dir.entryInfoList(QDir::Files);
        for(auto & file_info : file_infos)
        {
            auto path = file_info.canonicalFilePath().toStdString();

            qDebug() << "Found file:" << path.c_str();

            int64_t date = file_info.lastModified().toMSecsSinceEpoch();

            if (!indexedPaths.count(path) || indexedPaths[path] != date)
            {
                qDebug() << "Indexing file:" << path.c_str();
                index_markdown(path, d_xdb);
            }

            indexedPaths.erase(path);
        }
    }

    {
        for (const auto & entry : indexedPaths)
        {
            auto & path = entry.first;

            qDebug() << "Removing doc: " << path.c_str();
            string id = string("id:") + Document::id_from_path(path);
            d_xdb.delete_document(id);
        }
    }
}

}
