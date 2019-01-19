#include "main_win.h"
#include "search.h"

#include <QApplication>

using namespace Notebook;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<Notebook::Search_Result>("Search_Result");

    Main_Window win;

    if (app.arguments().size() >= 3)
    {
        win.openDatabase(app.arguments()[1], app.arguments()[2]);
    }

    win.show();

    int status = app.exec();

    return status;
}
