#include "Pong.hpp"
#include "Client.hpp"

#include <thread>
#include <atomic>
#include <iostream>


// Initialise ip and port
const std::string IP = "127.0.0.1";
constexpr unsigned short TCP_PORT = 53000;
constexpr unsigned short UDP_PORT = 53001;

int main()
{
	std::atomic<bool> game_started{ false };

	// Initialise the client and Game
	Client client(IP, TCP_PORT, UDP_PORT);
	client.run();

	std::cout << "Press Enter to exit..." << std::endl;
	std::cin.get();

	return 0;
}