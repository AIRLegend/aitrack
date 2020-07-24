#pragma once

#include <Winsock2.h>
#include <Ws2tcpip.h>


#define IP "192.168.1.137"
#define PORT 5555







class UDPSender
{
private:
	const int BUFFER_SIZE = sizeof(double) * 6;
	double position_data[6];

	sockaddr_in dest;
	sockaddr_in local;

	WSAData data;
	SOCKET s;

public:
	UDPSender(const char* dest_ip, int dest_port);
	~UDPSender();
	void send_data( double* data);
};

