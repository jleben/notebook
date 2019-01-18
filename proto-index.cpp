#include <xapian.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "index_markdown.h"
#include "filesystem.h"

using namespace std;
#if 0
void index_file(const string & file_name, Xapian::WritableDatabase & db)
{
    string base_file_name = get_base_file_name(file_name);
    if (base_file_name.empty())
    {
        throw std::runtime_error(string("Base file name is empty for file: ") + file_name);
    }

    cout << "Document base file name: " << base_file_name << endl;

    string text;

    {
        ifstream file(file_name);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file.");
        }
        text = string(istreambuf_iterator<char>(file), {});
    }

    if (text.empty())
        return;

    Xapian::Document doc;

    {
        Xapian::TermGenerator term_gen;
        term_gen.set_document(doc);
        term_gen.index_text(text);
    }

    doc.add_value(0, file_name);

    int id = id_for_file_path(base_file_name);

    cout << "Storing document id: " << id << endl;

    db.replace_document(id, doc);
}
#endif
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Expected arguments: <database dir> <files to index>" << endl;
        return 1;
    }

    string db_dir_name(argv[1]);

    vector<string> file_paths;

    for (int i = 2; i < argc; ++i)
    {
        file_paths.push_back(argv[i]);
    }

    Xapian::WritableDatabase db;

    try
    {
        db = Xapian::WritableDatabase(db_dir_name);
    }
    catch(Xapian::Error & e)
    {
        cerr << "Failed to open database: " << e.get_msg() << endl;
        return 1;
    }

    for (auto & file_path : file_paths)
    {
        cout << "Indexing file: " << file_path << endl;

        if (file_name_extension(file_name(file_path)) != "md")
        {
            cout << "Not a Markdown file. Skipping." << endl;
            continue;
        }

        try
        {
            index_markdown(file_path, db);
        }
        catch (std::runtime_error & e)
        {
            cerr << "Failed to index file: " << e.what() << endl;
        }
    }
}
