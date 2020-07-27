#include "IPResolver.h"

#include <QTcpSocket>
#include <QHostAddress>
//#include <iostream>

std::string network::get_local_ip()
{
    QTcpSocket socket;
    std::string ip = "127.0.0.1";
    socket.connectToHost("8.8.8.8", 53); // google DNS
    if (socket.waitForConnected()) {
        ip = socket.localAddress().toString().toStdString();
    }
    return ip;
}