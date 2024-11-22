#include "Client.hpp"
#include <iostream>
#include "Pong.hpp"

void Client::run() {
    // 1 - Connect to server
    connectToServer();

    // 2 - Bind UDP socket
    if (udp_socket_.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Binding UDP socket failed" << std::endl;
        return;
    }

    // 3 - Send client port to server
    sf::Packet clientPortPacket;
    clientPortPacket << udp_socket_.getLocalPort();
    if (tcp_socket_->send(clientPortPacket) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Sending client port failed" << std::endl;
        return;
    }


    // 4 - receive start message using TCP
    sf::Packet packet;
    if (tcp_socket_->receive(packet) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving start message failed" << std::endl;
        return;
    }

    if (std::string message; !(packet >> message) || message != "start") {
        std::cerr << "[ERROR] - Invalid start message" << std::endl;
        return;
    }

    // 5 - Run the game
    game_.run();
}

void Client::connectToServer() {
    if (tcp_socket_->connect(server_ip_, server_tcp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Connecting: disconnected or some other error" << std::endl;
    } else {
        std::cout << "Connected to server via TCP" << std::endl;
    }

    udp_socket_.setBlocking(false);
}

void Client::sendPaddlePosition(const sf::Vector2f &position) {
    sf::Packet packet;
    packet << position.x << position.y;
    if (udp_socket_.send(packet, server_ip_, server_udp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Sending paddle position" << std::endl;
    }
}
