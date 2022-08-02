#pragma once

#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <Ws2def.h>

#include <string>

#define UDPSENDER_SUPPORT_IPv6 1

/**
	Data sender to Opentrack using UDP
*/
class UDPSender
{
private:
	const int BUFFER_SIZE = sizeof(double) * 6;
	double position_data[6];

#ifdef UDPSENDER_SUPPORT_IPv6
	union
	{
		sockaddr_in  dest;
		sockaddr_in6 dest_IPv6;
	};
	union
	{
		sockaddr_in  local;
		sockaddr_in6 local_IPv6;
	};
#else
	sockaddr_in dest;
	sockaddr_in local;
#endif

	WSAData data;
	SOCKET s;

public:
	std::string ip;
	int port;

	UDPSender(const char* dest_ip, int dest_port);
	~UDPSender();

	/**
		Sends a data vector to opentrack.
		@param data: Size 6 array which contains [X,Y,Z,Yaw,Pitch,Roll].
	*/
	void send_data(double* data);
};