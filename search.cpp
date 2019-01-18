#include "search.h"
#include "document.h"

namespace Notebook {

Search_Result search(Xapian::Database & db, Xapian::Query & query, int max_results)
{
    Xapian::Enquire enquire(db);
    enquire.set_query(query);

    vector<Found_Document> found_docs;

    auto matches = enquire.get_mset(0, max_results);
    for (auto match = matches.begin(); match != matches.end(); ++match)
    {
        auto doc = match.get_document();

        string path = doc.get_value(Document::Path);
        string title = doc.get_value(Document::Title);

        Found_Document found_doc(doc.get_docid(), path, title);
        found_docs.push_back(found_doc);
    }

    return Search_Result(found_docs);
}

}
