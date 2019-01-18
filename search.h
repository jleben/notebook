#pragma once

#include <xapian.h>
#include <string>
#include <vector>

namespace Notebook {

using std::string;
using std::vector;

class Found_Document
{
public:
    Found_Document(int id, const string & path, const string & title):
        d_id(id),
        d_path(path),
        d_title(title)
    {}

    int id() const { return d_id; }
    const string & path() const { return d_path; }
    const string & title() const { return d_title; }

private:
    int d_id;
    string d_path;
    string d_title;
};

class Search_Result
{
public:
    Search_Result(vector<Found_Document> docs): d_docs(docs) {}
    const vector<Found_Document> & documents() const { return d_docs; }

private:
    vector<Found_Document> d_docs;
};

Search_Result search(Xapian::Database & db, Xapian::Query & query, int max_results = 100);

}
