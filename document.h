#pragma once

#include <QTextLayout>
#include <QString>

#include <string>
#include <list>
#include <utility>

namespace Notebook {

using std::string;
using std::list;
using std::pair;
class Document;

class Document_Element
{
public:
    virtual ~Document_Element() {}
    virtual void setWidth(int width) = 0;
    virtual int height() = 0;
    virtual void draw(QPainter *, const QPointF & position) = 0;
};

class Text_Element : public Document_Element
{
public:
    Text_Element(Document *);

    enum Cursor_Direction
    {
        Cursor_Left_Char,
        Cursor_Right_Char,
        Cursor_Previous_Char,
        Cursor_Next_Char,
        Cursor_Start,
        Cursor_End
    };

    QString text() const { return d_text; }
    void setText(const QString & text);
    int insertText(int pos, const QString & text);
    void removeText(int start, int length);

    int length() const { return d_text.size(); }

    int cursorPosAtPoint(const QPoint &);
    int cursorPos() const { return d_cursor_pos; }
    void setCursorPos(int pos) { d_cursor_pos = pos; }
    int previousCursorPos(int pos);
    int nextCursorPos(int pos);

    void moveCursor(Cursor_Direction);

    void setFontSize(float size);

    virtual void setWidth(int width);
    virtual int height();
    virtual void draw(QPainter *, const QPointF & position);

    void requestUpdateLayout();

private:
    void updateLayout();

    Document * d_doc = nullptr;
    QString d_text;
    float d_font_size = 0;

    QTextLayout d_layout;
    bool d_layout_update_needed = true;
    double d_width = 0;
    double d_height = 0;

    int d_cursor_pos = -1;
};

class Document
{
public:
    using Element_Iterator = list<Document_Element*>::iterator;

    enum Field
    {
        Path = 0,
        Title = 1,
        Date = 2,
    };

    static string id_from_path(const string & path);

    Element_Iterator insertHeading(const QString & text, Element_Iterator pos);
    Element_Iterator insertParagraph(const QString & text, Element_Iterator pos);

    void setWidth(int width);
    int height();

    void draw(QPainter *, const QPointF & position);
    QFont font() const { return d_font; }

    bool empty() const { return d_elements.empty(); }

    Document_Element * first() { return empty() ? nullptr : d_elements.front(); }
    Document_Element * last() { return empty() ? nullptr : d_elements.back(); }

    Element_Iterator begin() { return d_elements.begin(); }
    Element_Iterator end() { return d_elements.end(); }

    pair<Element_Iterator, QPointF> elementAt(const QPointF & pos);

    void requestUpdateLayout() { d_layout_update_needed = true; }

private:
    void updateLayout();

    list<Document_Element*> d_elements;
    QFont d_font;
    int d_width = 0;
    int d_spacing = 30;

    bool d_layout_update_needed = true;
    int d_height = 0;
};

}
