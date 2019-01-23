#pragma once

#include <QTextDocument>
#include <iostream>

namespace Notebook {

using std::istream;

QTextDocument * markdown_to_document(istream & input);

}
