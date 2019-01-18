#pragma once

#include <xapian.h>
#include <string>

void index_markdown(const std::string & file_path, Xapian::WritableDatabase & db);

