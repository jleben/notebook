cmake_minimum_required(VERSION 3.10)

project(notebook)

set(CMARK_TESTS OFF CACHE BOOL "")
set(CMARK_SHARED OFF CACHE BOOL "")

add_subdirectory(cmark)

include_directories(cmark/src)
include_directories(${CMAKE_BINARY_DIR}/cmark/src)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

#add_executable(notebook-index proto-index.cpp index_markdown.cpp)
#add_executable(notebook-search proto-search.cpp)
#target_link_libraries(notebook-index xapian)
#target_link_libraries(notebook-search xapian)

set(notebook_base_sources
    main_win.cpp
    search_results_view.cpp
    note_editor.cpp
    markdown_document.cpp
    search.cpp
    database.cpp
    document.cpp
    index_markdown.cpp
)

add_library(notebook_base OBJECT ${notebook_base_sources})
set_property(TARGET notebook_base PROPERTY CXX_STANDARD 17)
target_include_directories(notebook_base PRIVATE $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_definitions(notebook_base PRIVATE $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_COMPILE_DEFINITIONS>)
target_compile_options(notebook_base PRIVATE $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_COMPILE_OPTIONS>)
target_compile_features(notebook_base PRIVATE $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_COMPILE_FEATURES>)

add_executable(notebook
    main.cpp
    $<TARGET_OBJECTS:notebook_base>
)

set_property(TARGET notebook PROPERTY CXX_STANDARD 17)
target_link_libraries(notebook xapian libcmark_static Qt5::Widgets)

add_subdirectory(test)
