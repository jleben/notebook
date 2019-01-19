#include "index_markdown.h"
#include "document.h"
#include "filesystem.h"

#include <fstream>
#include <iostream>
#include <regex>

#include <QFileInfo>
#include <QDateTime>

using namespace std;

int id_for_string(const string & text)
{
    auto S = sizeof(int);
    int count = text.size() / S;
    int * data = (int*)text.data();
    int id = 0;
    for(int i = 0; i < count; ++i)
    {
        id = id xor data[i];
    }
    return id;
}

int id_for_file_path(const string & file_path)
{
    return id_for_string(file_path);
}

void index_markdown(const string & raw_file_path, Xapian::WritableDatabase & db)
{
    QFileInfo info(QString::fromStdString(raw_file_path));

    string file_path = info.canonicalFilePath().toStdString();
    string date = std::to_string(info.lastModified().toMSecsSinceEpoch());

    string id = string("id:") + Notebook::Document::id_from_path(file_path);

    Xapian::Document doc;

    doc.add_term(id);

    Xapian::TermGenerator term_gen;
    term_gen.set_document(doc);

    ifstream input(file_path);

    std::regex header_regx("^\\s*(#+)\\s+(.+)");

    string first_header;

    string line;
    while(std::getline(input, line))
    {
        if (line.empty())
        {
            term_gen.increase_termpos();
            continue;
        }

        std::smatch header_match;

        if (regex_search(line, header_match, header_regx))
        {
            //cout << endl << line << endl;
            //cout << "Prefix: " << header_match.str(1) << endl;
            //cout << "Text: " << header_match.str(2) << endl;

            if (first_header.empty())
            {
                first_header = header_match.str(2);
            }

            int level = header_match.length(1);
            int weight = std::max(1, 100 / level);

            //cout << "Header weight: " << weight << endl;
            //cout << line << endl;

            term_gen.index_text(line, weight);
        }
        else
        {
            term_gen.index_text(line);
        }
    }

    doc.add_value(Notebook::Document::Path, file_path);

    if (!first_header.empty())
    {
        doc.add_value(Notebook::Document::Title, first_header);
    }

    doc.add_value(Notebook::Document::Date, date);

    cout << "Storing document:" << endl
         << " id = " << id << endl
         << " path = " << file_path << endl
         << " title = " << first_header << endl
         << " date = " << date << endl
         ;

    db.replace_document(id, doc);
}

