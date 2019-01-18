#include <xapian.h>
#include <fstream>
#include <iostream>

using namespace std;

void print_snippet(Xapian::MSetIterator & match, Xapian::MSet & mset)
{
    auto doc = match.get_document();

    string file_name = doc.get_value(0);

    cout << endl << "-----"
         << " r: " << match.get_rank()
         << " w: " << match.get_weight()
         << file_name << ":" << endl;

    string text;

    {
        ifstream file(file_name);
        text = string(istreambuf_iterator<char>(file), {});
    }

    auto snippet = mset.snippet(text, 50);

    cout << snippet << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Expected arguments: <db> <query>" << endl;
        return 1;
    }

    string db_dir_name(argv[1]);

    Xapian::Database db;

    try
    {
        db = Xapian::Database (db_dir_name);
    }
    catch (Xapian::Error & e)
    {
        cerr << "Failed to open database: " << e.get_msg() << endl;
        return 1;
    }

    Xapian::QueryParser query_parser;
    query_parser.set_database(db);
    auto query = query_parser.parse_query(argv[2]);

    Xapian::Enquire enquire(db);
    enquire.set_query(query);

    auto matches = enquire.get_mset(0, 10);
    for (auto match_it = matches.begin(); match_it != matches.end(); ++match_it)
    {
        print_snippet(match_it, matches);
    }
}
