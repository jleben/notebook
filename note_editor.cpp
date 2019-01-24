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
        cout << "Clicked element. Cursor pos = " << cursorPos << endl;
    }

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

        update();
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

void Note_Editor::resizeEvent(QResizeEvent*)
{
    d_doc->setWidth(width());
    update();
}

void Note_Editor::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);

    d_doc->draw(&painter, QPointF(0,0));

    //painter.drawText(rect(), Qt::TextWordWrap, d_text);
}

}
