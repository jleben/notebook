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
    Document * d_doc = nullptr;
    Document::Element_Iterator d_current_elem;
    float d_scroll_y = 0;
};

}
