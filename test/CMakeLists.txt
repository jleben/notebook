
file(COPY test.md DESTINATION .)

add_executable(test_notebook
    test_main.cpp
    test_markdown_document.cpp
    $<TARGET_OBJECTS:notebook_base>
)

set_property(TARGET notebook PROPERTY CXX_STANDARD 17)
target_link_libraries(test_notebook xapian libcmark_static Qt5::Widgets)
