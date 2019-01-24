#pragma once

#include <QTextLayout>
#include <QString>

#include <string>
#include <list>

namespace Notebook {

using std::string;
using std::list;
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

    QString text() const { return d_text; }
    void setText(const QString & text);
    void insertText(int pos, const QString & text);
    int length() const { return d_text.size(); }

    void setFontSize(float size);

    virtual void setWidth(int width);
    virtual int height();
    virtual void draw(QPainter *, const QPointF & position);

private:
    void updateLayout();

    Document * d_doc = nullptr;
    QString d_text;
    float d_font_size = 0;

    QTextLayout d_layout;
    bool d_layout_update_needed = true;
    double d_width = 0;
    double d_height = 0;
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

    void insertHeading(const QString & text);
    void insertParagraph(const QString & text);

    void setWidth(int width);
    void draw(QPainter *, const QPointF & position);
    QFont font() const { return d_font; }

    bool empty() const { return d_elements.empty(); }

    Document_Element * first() { return empty() ? nullptr : d_elements.front(); }
    Document_Element * last() { return empty() ? nullptr : d_elements.back(); }

    Element_Iterator begin() { return d_elements.begin(); }
    Element_Iterator end() { return d_elements.end(); }

    Element_Iterator elementAt(const QPointF & pos);

private:
    list<Document_Element*> d_elements;
    QFont d_font;
    int d_width = 0;
    int d_spacing = 30;
};

}
