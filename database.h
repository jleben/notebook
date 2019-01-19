#pragma once

#include "search.h"

#include <QObject>
#include <QThread>
#include <QEvent>

namespace Notebook {

class Database_Worker;

class Database : public QObject
{
    Q_OBJECT

public:
    Database(const QString & index_path, const QString & docs_path);
    ~Database();

    QString index_path() const { return d_index_path; }
    QString docs_path() const { return d_docs_path; }

    void search(const QString & query);

signals:
    void searchFinished(Search_Result);

private:
    QString d_index_path;
    QString d_docs_path;

    Xapian::WritableDatabase d_db;
    QThread d_work_thread;
    Database_Worker * d_worker = nullptr;
};

class Database_Worker : public QObject
{
    Q_OBJECT

public:
    Database_Worker(Database * db);

    enum Event_Type
    {
        Search_Request_Event = QEvent::User
    };

    struct Search_Request : public QEvent
    {
        Search_Request(const QString & q):
            QEvent((QEvent::Type)Search_Request_Event), query_text(q) {}
        QString query_text;
    };

signals:
    void searchFinished(Search_Result);

private:
    void customEvent(QEvent *);
    void search(const Search_Request &);

    Database * d_db = nullptr;
    Xapian::Database d_xdb;
};

}
