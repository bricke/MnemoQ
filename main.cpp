#include <QCoreApplication>
#include "mnemoq.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Mnemoq mnemosyne;

    return a.exec();
}
