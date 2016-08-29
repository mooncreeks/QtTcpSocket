#include <QCoreApplication>
#include "qtTcpSocket.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtTcpSocket mTest;
    mTest.doConnect();
    mTest.sendCommand("cmd");
    return a.exec();
}
