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


    // 4 - receive start message from server using TCP
    sf::Packet packet;
    if (tcp_socket_->receive(packet) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving start message failed" << std::endl;
        return;
    }

    int paddleIndex;
    if (std::string message; !(packet >> paddleIndex >> message) || message != "start") {
        std::cerr << "[ERROR] - Invalid start message" << std::endl;
        return;
    }
    paddleIndex_ = paddleIndex;

    // 5 - Run the game
    Pong game(true, this);
    while (true) {
        game.run();
    }
}

void Client::connectToServer() {
    if (tcp_socket_->connect(server_ip_, server_tcp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Connecting: disconnected or some other error" << std::endl;
    } else {
        std::cout << "Connected to server via TCP" << std::endl;
    }

    udp_socket_.setBlocking(false);
}

// Used inside game class
void Client::sendPaddlePosition(const sf::Vector2f &position) {
    sf::Packet packet;
    packet << position.x << position.y;
    if (udp_socket_.send(packet, server_ip_, server_udp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Sending paddle position" << std::endl;
    }
}

// TODO: Finish this off
bool Client::receiveOtherPlayerData(sf::Vector2f &ball_position, sf::Vector2f &other_paddle_position) {
    sf::Packet packet;
    if (udp_socket_.receive(packet, server_ip_, server_udp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving data from server" << std::endl;
        return false;
    }

    int message_type;
    if (packet >> message_type) {
        if (message_type == 0) {
            if (packet >> other_paddle_position.x >> ball_position.y) {
                return true;
            }
        }
    }

    return true;
}
