cmake_minimum_required(VERSION 3.10)

project(notebook)

add_executable(notebook-index index.cpp index_markdown.cpp)
add_executable(notebook-search search.cpp)

target_link_libraries(notebook-index xapian)
target_link_libraries(notebook-search xapian)
