#include "note_editor.h"
#include "document.h"

#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>

namespace Notebook {

Note_Editor::Note_Editor(QWidget * parent):
    QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    d_doc = new Document;
    d_current_elem = d_doc->end();

    d_doc->insertHeading("hohoho.");
    d_doc->insertParagraph("hahaha ha ha ha hahaha ha ha ha hahaha ha hohoho ho ho ho ho hohoho ho!");
}

void Note_Editor::mousePressEvent(QMouseEvent *event)
{
    d_current_elem = d_doc->elementAt(event->pos() + QPoint(0, d_scroll_y));
}

void Note_Editor::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        d_doc->insertParagraph("");
        update();
        return;
    }
    default:
    {
        if (event->text().isEmpty())
            return;

        if (d_current_elem == d_doc->end())
            return;

        auto e = *d_current_elem;
        auto p = dynamic_cast<Text_Element*>(e);
        if (!p)
            return;

        p->insertText(p->length(), event->text());

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
