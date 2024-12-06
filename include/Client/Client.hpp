#pragma once

#include "Pong.hpp"
#include <SFML/Network.hpp>
#include <memory>

class Client
{
public:
	Client(const std::string &ip, const unsigned short tcp_port, const unsigned short udp_port) : server_ip_(ip), server_tcp_port_(tcp_port), server_udp_port_(udp_port), paddle_index_() {}

	void run();

	// Getters
	[[nodiscard]] size_t getPaddleIndex() const { return paddle_index_; }

private:
	void connectToServer() const;
	void queueInput(const std::string& input);
	void sendInputsToServerInOnePacket();
	void sendInputsToServer(const std::string& input);
	void receiveSnapshotFromServer();


	std::unique_ptr<sf::TcpSocket> tcp_socket_ = std::make_unique<sf::TcpSocket>();
	sf::UdpSocket udp_socket_;
	sf::IpAddress server_ip_;
	unsigned short server_tcp_port_;
	unsigned short server_udp_port_;

	std::vector<std::string> input_queue_;

	size_t paddle_index_;

	std::unique_ptr<Pong> game_;
};