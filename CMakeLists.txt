cmake_minimum_required(VERSION 3.10)

project(notebook)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

add_executable(notebook-index proto-index.cpp index_markdown.cpp)
add_executable(notebook-search proto-search.cpp)

target_link_libraries(notebook-index xapian)
target_link_libraries(notebook-search xapian)

add_executable(notebook
    main.cpp
    main_win.cpp
    search_results_view.cpp
    search.cpp
    database.cpp
    index_markdown.cpp
)
set_property(TARGET notebook PROPERTY CXX_STANDARD 17)
target_link_libraries(notebook xapian Qt5::Widgets)
