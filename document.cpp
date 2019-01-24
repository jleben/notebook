#include "document.h"
#include "filesystem.h"

#include <QFontMetrics>

namespace Notebook {

Text_Element::Text_Element(Document * doc):
    d_doc(doc)
{

}

void Text_Element::requestUpdateLayout()
{
    d_layout_update_needed = true;
    d_doc->requestUpdateLayout();
}

void Text_Element::setWidth(int width)
{
    d_width = width;
    requestUpdateLayout();
}

void Text_Element::setText(const QString & text)
{
    d_text = text;
    requestUpdateLayout();
}

int Text_Element::insertText(int pos, const QString & text)
{
    if (pos < 0 || pos > d_text.size())
        return -1;

    d_text.insert(pos, text);
    requestUpdateLayout();
    return pos + text.size();
}

void Text_Element::removeText(int start, int length)
{
    d_text.remove(start, length);
    requestUpdateLayout();
}

void Text_Element::setFontSize(float size)
{
    d_font_size = size;
    requestUpdateLayout();
}

int Text_Element::height()
{
    updateLayout();
    return d_height;
}

int Text_Element::cursorPosAtPoint(const QPoint & point)
{
    updateLayout();

    QPointF relPoint = QPointF(point) - d_layout.position();

    for (int i = 0; i < d_layout.lineCount(); ++i)
    {
        auto line = d_layout.lineAt(i);
        if (line.y() + line.height() > relPoint.y())
        {
            return line.xToCursor(relPoint.x());
        }
    }

    return -1;
}

void Text_Element::setCursorPos(int pos, int selection_size)
{
    d_cursor_pos = pos;
    d_selection_size = selection_size;
}

int Text_Element::previousCursorPos(int pos)
{
    updateLayout();

    return d_layout.previousCursorPosition(pos);
}

int Text_Element::nextCursorPos(int pos)
{
    updateLayout();

    return d_layout.nextCursorPosition(pos);
}

void Text_Element::moveCursor(Cursor_Direction dir)
{
    updateLayout();

    d_selection_size = 0;

    if (!d_layout.isValidCursorPosition(d_cursor_pos))
        return;

    switch(dir)
    {
    case Cursor_Left_Char:
        d_cursor_pos = d_layout.leftCursorPosition(d_cursor_pos);
        break;
    case Cursor_Right_Char:
        d_cursor_pos = d_layout.rightCursorPosition(d_cursor_pos);
        break;
    case Cursor_Previous_Char:
        d_cursor_pos = d_layout.previousCursorPosition(d_cursor_pos);
        break;
    case Cursor_Next_Char:
        d_cursor_pos = d_layout.nextCursorPosition(d_cursor_pos);
        break;
     case Cursor_Start:
        d_cursor_pos = 0;
        break;
    case Cursor_End:
        d_cursor_pos = d_text.size();
        break;
    }
}

void Text_Element::updateLayout()
{
    if (!d_layout_update_needed)
        return;

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
    updateLayout();

    QVector<QTextLayout::FormatRange> selections;
    if (d_cursor_pos >= 0 && d_selection_size > 0)
    {
        QTextLayout::FormatRange selection;
        selection.start = d_cursor_pos;
        selection.length = d_selection_size;
        selection.format.setBackground(Qt::blue);
        selections.push_back(selection);
    }

    d_layout.draw(painter, position, selections);

    if (d_cursor_pos >= 0)
    {
        d_layout.drawCursor(painter, position, d_cursor_pos, 2);
    }
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

void Document::updateLayout()
{
    if (!d_layout_update_needed)
        return;

    int height = 0;
    for (auto * elem : d_elements)
    {
        if (height > 0)
            height += d_spacing;

        height += elem->height();
    }
    d_height = height;

    d_layout_update_needed = false;
}

int Document::height()
{
    updateLayout();
    return d_height;
}

Document::Element_Iterator Document::insertHeading(const QString & text, Element_Iterator pos)
{
    auto elem = new Text_Element(this);
    elem->setText(text);
    elem->setFontSize(d_font.pointSizeF() * 2);
    elem->setWidth(d_width);

    return d_elements.insert(pos, elem);
}

Document::Element_Iterator Document::insertParagraph(const QString & text, Element_Iterator pos)
{
    auto elem = new Text_Element(this);
    elem->setText(text);
    elem->setWidth(d_width);

    return d_elements.insert(pos, elem);
}

pair<Document::Element_Iterator, QPointF> Document::elementAt(const QPointF & pos)
{
    if (pos.y() < 0)
        return { end(), QPointF() };

    int elem_y = 0;

    for(auto elem_it = begin(); elem_it != end(); ++elem_it)
    {
        int bottom = elem_y + (*elem_it)->height();

        if (bottom > pos.y())
            return { elem_it, QPointF(0, elem_y) };

        elem_y = bottom + d_spacing;
    }

    return { end(), QPointF() };
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
