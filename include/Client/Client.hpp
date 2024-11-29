#pragma once

#include <SFML/Network.hpp>
#include <memory>

#include "Pong.hpp"

class Client
{
public:
	Client(const std::string ip, unsigned short tcp_port, unsigned short udp_port) : server_ip_(ip), server_tcp_port_(tcp_port), server_udp_port_(udp_port) {}

	void run();
	void sendPaddlePosition(const sf::Vector2f& position);
	bool receiveOtherPlayerData(sf::Vector2f& ball_position, sf::Vector2f& other_paddle_position);

	int getPaddleIndex() const { return paddleIndex_; }

private:
	void connectToServer();


	std::unique_ptr<sf::TcpSocket> tcp_socket_ = std::make_unique<sf::TcpSocket>();
	sf::UdpSocket udp_socket_;
	sf::IpAddress server_ip_;
	unsigned short server_tcp_port_;
	unsigned short server_udp_port_;

	int paddleIndex_;
};