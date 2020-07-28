#include "UDPSender.h"



#include <iostream>
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")


UDPSender::UDPSender(const char* dest_ip, int dest_port)
{

    this->ip = std::string(dest_ip);

    std::cout << "from sender, the ip is " << this->ip << std::endl;

    this->port = dest_port;

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
    closesocket(s);
    WSACleanup();
}


void UDPSender::send_data( double* d)
{
    // Make packet

    //double d2[] = { 10.0,3.0,3.0,    15.0, 3.0, 3.0 };

    const char* pkt = (char*)d;

    sendto(s, pkt, BUFFER_SIZE, 0, (sockaddr*)&dest, sizeof(dest));



    //std::cout << "SENT PACKET" << std::endl;
    std::cin;

};



