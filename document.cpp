#include "document.h"
#include "filesystem.h"

#include <QFontMetrics>

namespace Notebook {

Text_Element::Text_Element(Document * doc):
    d_doc(doc)
{

}

void Text_Element::setWidth(int width)
{
    d_width = width;
    d_layout_update_needed = true;
}

void Text_Element::setText(const QString & text)
{
    d_text = text;
    d_layout_update_needed = true;
}

void Text_Element::insertText(int pos, const QString & text)
{
    if (pos >= 0 && pos <= d_text.size())
        d_text.insert(pos, text);
    d_layout_update_needed = true;
}

void Text_Element::setFontSize(float size)
{
    d_font_size = size;
    d_layout_update_needed = true;
}

int Text_Element::height()
{
    if (d_layout_update_needed)
        updateLayout();

    return d_height;
}

void Text_Element::updateLayout()
{
    {
        auto f = d_doc->font();
        if (d_font_size > 0)
            f.setPointSizeF(d_font_size);

        d_layout.setFont(f);
    }

    d_layout.setText(d_text);
    d_layout.setCacheEnabled(true);

    QFontMetrics fm(d_layout.font());

    int leading = fm.leading();
    double height = 0;

    d_layout.beginLayout();

    while (1)
    {
        QTextLine line = d_layout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(d_width);
        height += leading;

        line.setPosition(QPointF(0, height));
        height += line.height();
    }

    d_layout.endLayout();

    d_height = height;

    d_layout_update_needed = false;
}

void Text_Element::draw(QPainter * painter, const QPointF & position)
{
    if (d_layout_update_needed)
        updateLayout();

    d_layout.draw(painter, position);
}

string Document::id_from_path(const string & path)
{
    return file_name_base(file_name(path));
}

void Document::setWidth(int width)
{
    d_width = width;

    for (auto * elem : d_elements)
    {
        elem->setWidth(width);
    }
}

void Document::insertHeading(const QString & text)
{
    auto elem = new Text_Element(this);
    elem->setText(text);
    elem->setFontSize(d_font.pointSizeF() * 2);
    elem->setWidth(d_width);

    d_elements.push_back(elem);
}

void Document::insertParagraph(const QString & text)
{
    auto elem = new Text_Element(this);
    elem->setText(text);
    elem->setWidth(d_width);

    d_elements.push_back(elem);
}

Document::Element_Iterator Document::elementAt(const QPointF & pos)
{
    if (pos.y() < 0)
        return end();

    int elem_y = 0;

    for(auto elem_it = begin(); elem_it != end(); ++elem_it)
    {
        elem_y += (*elem_it)->height();

        if (elem_y > pos.y())
            return elem_it;

        elem_y += d_spacing;
    }

    return end();
}

void Document::draw(QPainter * painter, const QPointF & position)
{
    QPointF p = position;

    for (auto * elem : d_elements)
    {
        elem->draw(painter, p);
        p += QPointF(0, elem->height() + d_spacing);
    }
}

}
