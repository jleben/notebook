#include "markdown_document.h"

#include <cmark.h>
#include <iostream>
#include <stack>
#include <sstream>

#include <QTextCursor>
#include <QTextBlockFormat>
#include <QTextCharFormat>

using namespace std;

namespace Notebook {

static
void print_markdown_ast(cmark_node * root)
{
    cmark_iter *iter = cmark_iter_new(root);
    cmark_event_type event;
    while ((event = cmark_iter_next(iter)) != CMARK_EVENT_DONE)
    {
        switch(event)
        {
        case CMARK_EVENT_NONE:
            cout << "none "; break;
        case CMARK_EVENT_ENTER:
            cout << "enter "; break;
        case CMARK_EVENT_EXIT:
            cout << "exit "; break;
        case CMARK_EVENT_DONE:
            cout << "done "; break;
        default:
            cout << "? ";
        }

        cmark_node *node = cmark_iter_get_node(iter);
        if (!node)
            cout << "<null>" << endl;
        else
            cout << cmark_node_get_type_string(node) << endl;
    }
    cmark_iter_free(iter);
}

static QTextBlockFormat defaultTextBlockFormat()
{
    QTextBlockFormat f;
    f.setTopMargin(5);
    f.setBottomMargin(20);
    return f;
}

static QTextCharFormat defaultCharFormat()
{
    return QTextCharFormat();
}

static QTextCharFormat headingCharFormat(int level)
{
    QTextCharFormat f;
    f.setFontPointSize((7-level) * 4);
    return f;
}

static QTextFrameFormat codeFrameFormat()
{
    QTextFrameFormat f;
    f.setBackground(QColor(200,200,200));
    f.setTopMargin(20);
    f.setBottomMargin(20);
    return f;
}

void throw_unexpected_node(const string & context, cmark_event_type event, cmark_iter * iter)
{
    string event_name;

    switch(event)
    {
    case CMARK_EVENT_NONE:
        event_name = "none "; break;
    case CMARK_EVENT_ENTER:
        event_name =  "enter "; break;
    case CMARK_EVENT_EXIT:
        event_name =  "exit "; break;
    case CMARK_EVENT_DONE:
        event_name = "done "; break;
    default:
        event_name = "? ";
    }

    ostringstream msg;
    msg << "Unexpected node in " << context << ": "
        << " event: "
        << event_name
        << " type: "
        << cmark_node_get_type_string(cmark_iter_get_node(iter));

    throw std::runtime_error(msg.str());
}

class Markdown_Parser
{
public:
    QTextDocument * markdown_to_document(istream & input)
    {
        string text(istreambuf_iterator<char>(input), {});

        int options = 0;
        auto root_node = cmark_parse_document(text.data(), text.size(), options);

        auto doc = new QTextDocument;
        QTextCursor cursor(doc);

        cmark_event_type event;
        cmark_iter *iter = cmark_iter_new(root_node);

        while ((event = cmark_iter_next(iter)) != CMARK_EVENT_DONE)
        {
            auto node = cmark_iter_get_node(iter);
            auto node_type = cmark_node_get_type(node);

            if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_DOCUMENT)
            {
                parse_document(iter, cursor);
            }
            else
            {
                throw_unexpected_node("root", event, iter);
            }
        }

        cmark_iter_free(iter);
        cmark_node_free(root_node);

        doc->clearUndoRedoStacks();

        return doc;
    }

private:
    stack<QTextCharFormat> d_char_formats;

    void parse_paragraph(cmark_iter * iter, QTextCursor & cursor)
    {
        if (cursor.positionInBlock() > 0)
            cursor.insertBlock();

        cursor.setBlockFormat(defaultTextBlockFormat());

        parse_text(iter, CMARK_NODE_PARAGRAPH, cursor);
    }

    void parse_heading(cmark_iter * iter, QTextCursor & cursor)
    {
        int level = cmark_node_get_heading_level(cmark_iter_get_node(iter));

        if (cursor.positionInBlock() > 0)
            cursor.insertBlock();

        cursor.setBlockFormat(defaultTextBlockFormat());

        auto f = headingCharFormat(level);

        d_char_formats.push(f);
        cursor.setCharFormat(f);

        parse_text(iter, CMARK_NODE_HEADING, cursor);

        d_char_formats.pop();
        cursor.setCharFormat(d_char_formats.top());
    }

    void parse_text(cmark_iter * iter, cmark_node_type parent_node_type, QTextCursor & cursor)
    {
        while(true)
        {
            auto event = cmark_iter_next(iter);
            auto node = cmark_iter_get_node(iter);
            auto node_type = cmark_node_get_type(node);

            if (event == CMARK_EVENT_EXIT && node_type == parent_node_type)
            {
                break;
            }
            else if (node_type == CMARK_NODE_TEXT)
            {
                auto text = cmark_node_get_literal(node);
                if (text)
                    cursor.insertText(text);
            }
            else if (node_type == CMARK_NODE_CODE)
            {
                auto text = cmark_node_get_literal(node);
                if (text)
                {
                    auto f = d_char_formats.top();
                    f.setFontFamily("monospace");
                    f.setBackground(QColor(200,200,200));
                    cursor.insertText(text, f);
                    cursor.setCharFormat(d_char_formats.top());
                }
            }
            else if (node_type == CMARK_NODE_SOFTBREAK)
            {
                cursor.insertText(" ");
            }
            else if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_EMPH)
            {
                auto f = d_char_formats.top();
                f.setFontItalic(true);
                d_char_formats.push(f);
                cursor.setCharFormat(f);
            }
            else if (event == CMARK_EVENT_EXIT && node_type == CMARK_NODE_EMPH)
            {
                d_char_formats.pop();
                cursor.setCharFormat(d_char_formats.top());
            }
            else if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_STRONG)
            {
                auto f = d_char_formats.top();
                f.setFontWeight(QFont::Bold);
                d_char_formats.push(f);
                cursor.setCharFormat(f);
            }
            else if (event == CMARK_EVENT_EXIT && node_type == CMARK_NODE_STRONG)
            {
                d_char_formats.pop();
                cursor.setCharFormat(d_char_formats.top());
            }
            else
            {
                throw_unexpected_node("text", event, iter);
            }
        }
    }

    void parse_code_block(cmark_iter * iter, QTextCursor & cursor)
    {
        cursor.insertFrame(codeFrameFormat());

        cursor.setBlockFormat(QTextBlockFormat());

        QTextCharFormat cf;
        cf.setFontFamily("monospace");

        d_char_formats.push(cf);
        cursor.setCharFormat(cf);

        QString text(cmark_node_get_literal(cmark_iter_get_node(iter)));
        if (text.endsWith('\n'))
            text.chop(1);

        cursor.insertText(text);

        cursor.movePosition(QTextCursor::NextBlock);

        d_char_formats.pop();
        cursor.setCharFormat(d_char_formats.top());
    }

    void parse_list(cmark_iter * iter, QTextCursor & cursor)
    {
        QTextListFormat lf;
        lf.setStyle(QTextListFormat::ListCircle);

        cursor.insertList(QTextListFormat::ListCircle);
        cursor.setBlockFormat(defaultTextBlockFormat());

        while(true)
        {
            auto event = cmark_iter_next(iter);
            auto node = cmark_iter_get_node(iter);
            auto node_type = cmark_node_get_type(node);

            if (event == CMARK_EVENT_EXIT && node_type == CMARK_NODE_LIST)
            {
                break;
            }
            else if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_ITEM)
            {
                cursor.insertText("blah");
                cursor.insertBlock();

                QTextCursor dummy;
                parse_list_item(iter, dummy);
            }
            else
            {
                throw_unexpected_node("list", event, iter);
            }
        }

        cursor.movePosition(QTextCursor::NextBlock);
    }

    void parse_list_item(cmark_iter * iter, QTextCursor & cursor)
    {
        while(true)
        {
            auto event = cmark_iter_next(iter);
            auto node = cmark_iter_get_node(iter);
            auto node_type = cmark_node_get_type(node);

            if (event == CMARK_EVENT_EXIT && node_type == CMARK_NODE_ITEM)
            {
                break;
            }
            else
            {
                parse_block(iter, cursor);
            }
        }
    }

    void parse_block(cmark_iter * iter, QTextCursor & cursor)
    {
        auto event = cmark_iter_get_event_type(iter);
        auto node_type = cmark_node_get_type(cmark_iter_get_node(iter));

        if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_HEADING)
        {
            parse_heading(iter, cursor);
        }
        else if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_PARAGRAPH)
        {
            parse_paragraph(iter, cursor);
        }
        else if (node_type == CMARK_NODE_CODE_BLOCK)
        {
            parse_code_block(iter, cursor);
        }
        else if (event == CMARK_EVENT_ENTER && node_type == CMARK_NODE_LIST)
        {
            parse_list(iter, cursor);
        }
        else
        {
            throw_unexpected_node("block", event, iter);
        }
    }

    void parse_document(cmark_iter * iter, QTextCursor & cursor)
    {
        d_char_formats.push(defaultCharFormat());

        while(true)
        {
            auto event = cmark_iter_next(iter);
            auto node_type = cmark_node_get_type(cmark_iter_get_node(iter));

            if (event == CMARK_EVENT_EXIT && node_type == CMARK_NODE_DOCUMENT)
            {
                break;
            }
            else
            {
                parse_block(iter, cursor);
            }
        }

        d_char_formats.pop();
    }
};

QTextDocument * markdown_to_document(istream & input)
{
    Markdown_Parser parser;
    return parser.markdown_to_document(input);
}

}
