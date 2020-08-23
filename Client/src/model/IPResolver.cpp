#include "IPResolver.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
//#include <iostream>

std::string network::get_local_ip()
{
    /*const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress& address : QNetworkInterface::allAddresses()) {
        
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
        {
#ifdef _DEBUG
            qDebug() << address.toString();
#endif // _DEBUG
            return address.toString().toStdString();
        }   
    }*/

    return "127.0.0.1";
}