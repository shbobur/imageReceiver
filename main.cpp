#include "receiverwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ReceiverWidget w;
    w.show();

    return a.exec();
}
