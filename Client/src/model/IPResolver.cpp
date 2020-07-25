#include "IPResolver.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <iostream>

std::string network::get_local_ip()
{
    QTcpSocket socket;
    socket.connectToHost("8.8.8.8", 53); // google DNS
    if (!socket.waitForConnected()) {
        std::cout << "ERROR OBTAINING IP" << std::endl;
    }

    return socket.localAddress().toString().toStdString();
}