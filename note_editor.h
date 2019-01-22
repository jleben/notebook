#pragma once

#include <QTextEdit>

namespace Notebook {

class Note_Editor : public QTextEdit
{
public:
    Note_Editor(QWidget * parent = nullptr);
};

}
