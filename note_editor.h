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
    virtual void keyPressEvent(QKeyEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);

private:
    Text_Element * currentTextElement();

    Document * d_doc = nullptr;
    float d_scroll_y = 0;
    Document::Element_Iterator d_current_elem;
};

}
