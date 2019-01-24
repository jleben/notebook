#pragma once

#include <QTextEdit>

namespace Notebook {

class Document;

class Note_Editor : public QWidget
{
public:
    Note_Editor(QWidget * parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);
private:
    Document * d_doc = nullptr;
};

}
