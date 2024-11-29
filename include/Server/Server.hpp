#pragma once
#include <SFML/Network.hpp>
#include <list>
#include <memory>

#include "Pong.hpp"

class Server
{
public:
	Server(const std::string ip, unsigned short tcp_port, unsigned short udp_port) : ip_(ip), tcp_port_(tcp_port), udp_port_(udp_port) {}

	void run();

private:
	sf::IpAddress ip_;
	unsigned short tcp_port_;
	unsigned short udp_port_;

	sf::TcpListener listener_;
	sf::SocketSelector selector_;
	
	std::list<std::unique_ptr<sf::TcpSocket>> clients_;
	std::vector<sf::IpAddress> client_ips_;
	std::vector<unsigned short> client_ports_;

	sf::UdpSocket udp_socket_;

	void listen();
	void handleNewConnections();

	sf::Packet receive();
	void send(const Pong &game);
	void handleDisconnect();
};
