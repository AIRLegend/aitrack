#include <iostream>

#include "UDPSender.h"

UDPSender::UDPSender(const char* dest_ip, int dest_port)
{

    this->ip = std::string(dest_ip);
    this->port = dest_port;

    //std::cout << "ip: " << this->ip << "  port: " <<  this->port << std::endl;

    dest = sockaddr_in();
    local = sockaddr_in();

    WSAStartup(MAKEWORD(2, 2), &data);

    local.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &local.sin_addr.s_addr);
    local.sin_port = htons(0);

    dest.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &dest.sin_addr.s_addr);
    dest.sin_port = htons(dest_port);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind(s, (sockaddr*)&local, sizeof(local));
}

UDPSender::~UDPSender()
{
    std::cout << "Closing connection" << std::endl;
    closesocket(s);
    WSACleanup();
}


void UDPSender::send_data(double* d)
{
    // Make packet
    const char* pkt = (char*)d;
    sendto(s, pkt, BUFFER_SIZE, 0, (sockaddr*)&dest, sizeof(dest));
};


