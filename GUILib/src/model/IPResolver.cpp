#include "IPResolver.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <iostream>

std::string network::get_local_ip()
{
    QTcpSocket socket;
    socket.connectToHost("8.8.8.8", 53); // google DNS, or something else reliable
    if (socket.waitForConnected()) {
        std::cout << socket.localAddress().toString().toStdString() << std::endl;
    }
    else {
        std::cout << "ERROR OBTAINING IP" << std::endl;
    }

    return socket.localAddress().toString().toStdString();
}