#pragma once

#include "document.h"

#include <QTextEdit>

namespace Notebook {

class Document;

class Note_Editor : public QWidget
{
public:
    Note_Editor(QWidget * parent = nullptr);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void wheelEvent(QWheelEvent *);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);

private:
    Text_Element * currentTextElement();

    void moveCursor(Text_Element::Cursor_Direction);
    void normalizeScroll();

    Document * d_doc = nullptr;
    int d_scroll_y = 0;
    Document::Element_Iterator d_current_elem;

    int d_selection_start = 0;
};

}
