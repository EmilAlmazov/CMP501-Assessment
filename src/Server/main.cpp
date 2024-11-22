#include "Server.hpp"

const std::string IP = "127.0.0.1";
constexpr unsigned short TCP_PORT = 53000;
constexpr unsigned short UDP_PORT = 53001;

int main()
{
	Server server(IP, TCP_PORT, UDP_PORT);
	server.run();

	return 0;
}