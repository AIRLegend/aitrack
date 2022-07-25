#include <iostream>

#include "UDPSender.h"

UDPSender::UDPSender(const char* dest_ip, int dest_port)
{
    this->ip = std::string(dest_ip);
    this->port = dest_port;

    //std::cout << "ip: " << this->ip << "  port: " <<  this->port << std::endl;

    WSAStartup(MAKEWORD(2, 2), &data);

#ifdef UDPSENDER_SUPPORT_IPv6
    memset(&dest_IPv6, 0, sizeof(dest_IPv6));
    memset(&local_IPv6, 0, sizeof(local_IPv6));

    dest_IPv6.sin6_port = htons(dest_port);
    if (dest_IPv6.sin6_port == 0)
        dest_IPv6.sin6_port = htons(4242);

    if (inet_pton(AF_INET6, dest_ip, &dest_IPv6.sin6_addr) == 1)
    {
        // valid IPv6 address
        dest_IPv6.sin6_family = AF_INET6;
        local_IPv6.sin6_family = AF_INET6;
        local_IPv6.sin6_addr = IN6ADDR_ANY_INIT;
        bind(s, (sockaddr*)&local_IPv6, sizeof(local_IPv6));
    }
    else if (inet_pton(AF_INET, dest_ip, &dest.sin_addr) == 1)
    {
        // valid IPv4 address
        dest.sin_family = AF_INET;
        local.sin_family = AF_INET;
        local.sin_addr.S_un.S_addr = INADDR_ANY;
        bind(s, (sockaddr*)&local, sizeof(local));
    }
    else
    {
        // invalid address, default to IPv4 LOOPBACK
        dest.sin_family = AF_INET;
        dest.sin_addr.S_un.S_addr = INADDR_LOOPBACK;
        local.sin_family = AF_INET;
        local.sin_addr.S_un.S_addr = INADDR_ANY;
        bind(s, (sockaddr*)&local, sizeof(local));
    }
#else
    dest = sockaddr_in();
    local = sockaddr_in();

    dest.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &dest.sin_addr.s_addr);
    dest.sin_port = htons(dest_port);

    local.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &local.sin_addr.s_addr);
    local.sin_port = htons(0);

    dest.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &dest.sin_addr.s_addr);
    dest.sin_port = htons(dest_port);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind(s, (sockaddr*)&local, sizeof(local));
#endif
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


