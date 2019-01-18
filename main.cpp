#include "main_win.h"

#include <QApplication>

using namespace Notebook;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Main_Window win;

    if (app.arguments().size() >= 2)
    {
        win.openDatabase(app.arguments()[1]);
    }

    win.show();

    int status = app.exec();

    return status;
}
