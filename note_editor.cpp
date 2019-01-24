#include "note_editor.h"
#include "document.h"

#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <iostream>

using namespace std;

namespace Notebook {

Note_Editor::Note_Editor(QWidget * parent):
    QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    d_doc = new Document;
    d_current_elem = d_doc->end();

    d_doc->insertHeading("hohoho.", d_doc->end());
    d_doc->insertParagraph("hahaha ha ha ha hahaha ha ha ha hahaha ha hohoho ho ho ho ho hohoho ho!",
                           d_doc->end());
}

Text_Element * Note_Editor::currentTextElement()
{
    if (d_current_elem == d_doc->end())
        return nullptr;

    return dynamic_cast<Text_Element*>(*d_current_elem);
}

void Note_Editor::mousePressEvent(QMouseEvent *event)
{
    auto te = currentTextElement();
    if (te)
    {
        te->setCursorPos(-1);
    }

    auto docPoint = event->pos() + QPoint(0, d_scroll_y);

    auto [elem, pos] = d_doc->elementAt(docPoint);
    {} // This fixes following auto-indentation in Qt Creator.

    d_current_elem = elem;

    te = currentTextElement();
    if (te)
    {
        auto elemPoint = docPoint - pos.toPoint();
        int cursorPos = te->cursorPosAtPoint(elemPoint);
        te->setCursorPos(cursorPos);
        d_selection_start = cursorPos;
        cout << "Clicked element. Cursor pos = " << cursorPos << endl;
    }

    update();
}

void Note_Editor::mouseMoveEvent(QMouseEvent *event)
{
    auto te = currentTextElement();
    if (!te)
        return;

    if (d_selection_start < 0)
        return;

    auto docPoint = event->pos() + QPoint(0, d_scroll_y);

    auto [elem, pos] = d_doc->elementAt(docPoint);
    {}{} // This fixes following auto-indentation in Qt Creator.
    if (elem != d_current_elem)
        return;

    auto elemPoint = docPoint - pos.toPoint();

    int selection_end = te->cursorPosAtPoint(elemPoint);

    int s = std::min(d_selection_start, selection_end);
    int l = std::abs(d_selection_start - selection_end);

    cout << "Setting cursor: " << s << " + " << l << endl;

    te->setCursorPos(s, l);

    update();
}

void Note_Editor::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        if (d_current_elem == d_doc->end())
            return;

        {
            auto te = currentTextElement();
            if (te)
                te->setCursorPos(-1);
        }

        auto elem_pos = d_current_elem;
        ++elem_pos;
        d_current_elem = d_doc->insertParagraph("", elem_pos);

        {
            currentTextElement()->setCursorPos(0);
        }

        update();
        return;
    }
    case Qt::Key_Left:
    {
        moveCursor(Text_Element::Cursor_Left_Char);
        return;
    }
    case Qt::Key_Right:
    {
        moveCursor(Text_Element::Cursor_Right_Char);
        return;
    }
    case Qt::Key_Home:
    {
        moveCursor(Text_Element::Cursor_Start);
        return;
    }
    case Qt::Key_End:
    {
        moveCursor(Text_Element::Cursor_End);
        return;
    }
    case Qt::Key_Backspace:
    {
        auto te = currentTextElement();
        if (!te)
            return;

        int cur = te->cursorPos();
        if (cur < 1)
            return;

        int prev = te->previousCursorPos(cur);
        te->removeText(prev, cur-prev);
        te->setCursorPos(prev);

        update();
        return;
    }
    default:
    {
        if (event->text().isEmpty())
            return;

        auto t = currentTextElement();
        if (!t)
            return;

        int c = t->insertText(t->cursorPos(), event->text());
        t->setCursorPos(c);

        update();
        return;
    }
    }
}

void Note_Editor::moveCursor(Text_Element::Cursor_Direction dir)
{
    auto te = currentTextElement();
    if (!te)
        return;

    te->moveCursor(dir);

    update();
}

void Note_Editor::normalizeScroll()
{
    int max_y = std::max(0, d_doc->height() - height());
    int y = std::max(0, std::min(max_y, d_scroll_y));

    if (y != d_scroll_y)
    {
        d_scroll_y = y;
        update();
    }
}


void Note_Editor::wheelEvent(QWheelEvent *event)
{
    int offset = logicalDpiY() * 0.2f * event->angleDelta().y() / 120.0f;

    d_scroll_y -= offset;

    normalizeScroll();

    cout << "Scroll: " << offset << " -> " << d_scroll_y << endl;

    update();
}

void Note_Editor::resizeEvent(QResizeEvent*)
{
    d_doc->setWidth(width());

    normalizeScroll();

    update();
}

void Note_Editor::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);

    d_doc->draw(&painter, QPointF(0, -d_scroll_y));
}

}
