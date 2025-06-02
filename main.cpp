#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "httpdata.h"


// Метод хуйни, сделан на коленке чтобы удобно пихать все данные в http пакет
HttpBody createHttpPacket()
{
	HttpBody httpPacket;
	httpPacket.header.connectionType = HttpConnectionType::CLOSE;
	httpPacket.header.dataType = HttpDataType::TEXT_HTML;
	httpPacket.header.hostname = "jetbrains.com";
	httpPacket.header.method = HttpMethod::GET;
	httpPacket.header.path = "/";
	httpPacket.header.userAgent = "Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5";
	httpPacket.data.clear();

	return httpPacket;
}


int main()
{
	WSAData wsaData{};
	SOCKET sd{};
	int result{0};

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		throw std::runtime_error("WSAStartup");
	}

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sd == INVALID_SOCKET)
	{
		throw std::runtime_error("socket()");
	}

	addrinfo hints{}, *res;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo("jetbrains.com", "80", &hints, &res);
	if (result != 0) {
		closesocket(sd);
		throw std::runtime_error("getaddrinfo() failed");
	}

	result = connect(sd, res->ai_addr, static_cast<int>(res->ai_addrlen));
	if (result == SOCKET_ERROR)
	{
		closesocket(sd);
		throw std::runtime_error("connect");
	}

	freeaddrinfo(res);

	try
	{
		HttpBody req = createHttpPacket();
		HttpBody response;

		std::vector<char> serialized = req.serialize();
		std::vector<char> deserialized(8192);

		send(sd, serialized.data(), static_cast<int>(serialized.size()), 0);

		recv(sd, deserialized.data(), static_cast<int>(deserialized.size()), 0);

		response.deserialize(deserialized);

		for (const auto &it : response.data)
		{
			std::cout << it << std::endl;
		}

		/*
		 *				-> header	(code, path, method ...)
		 *	response.
		 *				-> data (binary array of char)
		 */
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}


	return 0;
}
