#include "../markdown_document.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace Notebook {

void test_markdown_document()
{
    ifstream input("test.md");
    if (!input.is_open())
        throw std::runtime_error("Failed to open file.");

    markdown_to_document(input);
}

}
