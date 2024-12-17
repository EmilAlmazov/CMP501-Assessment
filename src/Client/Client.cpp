#include "Client.hpp"
#include "Snapshot.hpp"
#include <Server.hpp>
#include <iostream>

void Client::run()
{
    // 1 - Connect to server
    connectToServer();

    // 2 - Receive client index from server
    if (sf::Packet packet; tcp_socket_->receive(packet) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving client index failed" << std::endl;
        return;
    }
    else {
        packet >> paddle_index_;
        std::cout << "Received client index: " << paddle_index_ << std::endl;
    }

    // 3 - Receive server's UDP port
    if (sf::Packet packet; tcp_socket_->receive(packet) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving server's UDP port failed" << std::endl;
        return;
    }
    else {
        packet >> server_udp_port_;
        std::cout << "Received server's UDP port: " << server_udp_port_ << std::endl;
    }

    // 4 - Bind UDP socket
    if (udp_socket_.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Binding UDP socket failed" << std::endl;
        return;
    }
    std::cout << "UDP socket bound to port: " << udp_socket_.getLocalPort() << std::endl;
    udp_socket_.setBlocking(false);

    // 4.1 - Send client port to server
    sf::Packet clientPortPacket;
    clientPortPacket << udp_socket_.getLocalPort();
    if (tcp_socket_->send(clientPortPacket) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Sending client port failed" << std::endl;
        return;
    }
    std::cout << "Client port: " << udp_socket_.getLocalPort() << " sent to server" << std::endl;

    // 5 - receive start message from server using TCP
    if (sf::Packet packet; tcp_socket_->receive(packet) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving start message failed" << std::endl;
    }
    else {
        std::string message;
        size_t client_index;
        packet >> message >> client_index;
        std::cout << "Received start message: " << message << " " << client_index << "\n\n" << std::endl;
    }

    // 6 - Run the game
    game_ = std::make_unique<Pong>(false, "Client " + std::to_string(paddle_index_));
    sf::Clock clock;
    sf::Clock clock2;
    while (game_->windowIsOpen()) {
        for (auto event = sf::Event(); game_->getWindow().pollEvent(event);) {
            if (event.type == sf::Event::Closed) { game_->getWindow().close(); }
        }

        // sendInputsToServer(game_->getInput(paddle_index_));
        queueInput(game_->getInput(paddle_index_));

        if (size_t packetsPerSecond = 30; clock.getElapsedTime().asMilliseconds() >= 1000 / packetsPerSecond) {
            sendInputsToServerInOnePacket();
            clock.restart();
        }

        receiveSnapshotFromServer();
        game_->render();
    }
}

// === HELPER FUNCTIONS ===
void Client::connectToServer()
{
    if (tcp_socket_->connect(server_ip_, server_tcp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Connecting: disconnected or some other error" << std::endl;
    }
    else {
        std::cout << "Connected to server via TCP" << std::endl;
    }
}

// void Client::sendInputsToServer(const std::string& input)
// {
//     if (input.empty()) { return; }
//
//     sf::Packet packet;
//     packet << input;
//     std::cout << "Sending input: " << input << " to packet" << std::endl;
//     if (udp_socket_.send(packet, server_ip_, server_udp_port_) != sf::Socket::Done) {
//         std::cerr << "[ERROR] - Sending input to server" << std::endl;
//     }
// }

void Client::queueInput(const std::string& input)
{
    if (!input.empty()) { input_queue_.push_back(input); }
}

// Send player inputs to server
void Client::sendInputsToServerInOnePacket()
{
    if (input_queue_.empty()) { return; }

    sf::Packet packet;
    for (const auto& input: input_queue_) {
        // sent time so can fact-check prediction with server (not implemented)
        // packet << input << clock_.getElapsedTime().asMilliseconds();
        packet << input;
        std::cout << "Adding input: " << input << " to packet" << std::endl;
    }
    if (udp_socket_.send(packet, server_ip_, server_udp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Sending input to server" << std::endl;
    }
    std::cout << "Sent " << input_queue_.size() << " inputs to server" << std::endl;

    input_queue_.clear();
}

void Client::receiveSnapshotFromServer()
{
    sf::Packet packet;
    sf::IpAddress sender;
    unsigned short port;
    sf::Socket::Status status = udp_socket_.receive(packet, sender, port);
    if (status == sf::Socket::Done) {
        if (Snapshot snapshot; packet >> snapshot) {
            std::cout << "Received snapshot from server" << std::endl;
            std::cout << snapshot << std::endl;

            // Update game state
            game_->setLeftPaddlePosition(snapshot.getLeftPaddlePosition());
            game_->setRightPaddlePosition(snapshot.getRightPaddlePosition());
            game_->setBallPosition(snapshot.getBallPosition());
            game_->setBallSpeed(snapshot.getBallSpeed());
            game_->setLeftScore(snapshot.getLeftScore());
            game_->setRightScore(snapshot.getRightScore());
        }
        else {
            std::cerr << "[ERROR] - Reading snapshot from server failed" << std::endl;
        }
    }
}
