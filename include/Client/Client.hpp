#pragma once

#include <SFML/Network.hpp>
#include <atomic>
#include <memory>

#include "Pong.hpp"

class Client
{
public:
	Client(const std::string ip, unsigned short tcp_port, unsigned short udp_port) : server_ip_(ip), server_tcp_port_(tcp_port), server_udp_port_(udp_port) {}

	void run();

private:
	std::unique_ptr<sf::TcpSocket> tcp_socket_ = std::make_unique<sf::TcpSocket>();
	sf::UdpSocket udp_socket_;
	sf::IpAddress server_ip_;
	unsigned short server_tcp_port_;
	unsigned short server_udp_port_;

	// Pong game
	Pong game_{false};

	void connectToServer();
	void sendPaddlePosition(const sf::Vector2f& position);
};