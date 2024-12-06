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

    // 4.1 - Send client port to server
    sf::Packet clientPortPacket;
    clientPortPacket << udp_socket_.getLocalPort();
    if (tcp_socket_->send(clientPortPacket) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Sending client port failed" << std::endl;
        return;
    }
    std::cout << "Client port: " << udp_socket_.getLocalPort() << " sent to server" << std::endl;

    // 5 - receive start message from server using TCP
    if (sf::Packet packet; tcp_socket_->receive(packet) != sf::Socket::Done) { std::cerr << "[ERROR] - Receiving start message failed" << std::endl; }
    else {
        std::string message;
        size_t client_index;
        packet >> message >> client_index;
        std::cout << "Received start message: " << message << " " << client_index << std::endl;
    }

    // 6 - Run the game
    game_ = std::make_unique<Pong>(true, "Client " + std::to_string(paddle_index_));
    sf::Clock clock;
    while (game_->windowIsOpen()) {
        for (auto event = sf::Event(); game_->getWindow().pollEvent(event);) {
            if (event.type == sf::Event::Closed) { game_->getWindow().close(); }
        }

        sendInputsToServer(game_->getInput(paddle_index_));
        // queueInput(game_->getInput(paddle_index_));

        if (size_t packetsPerSecond = 30; clock.getElapsedTime().asMilliseconds() >= 1000 / packetsPerSecond) {
            // sendInputsToServerInOnePacket();
            clock.restart();
        }

        game_->render();
        receiveSnapshotFromServer();
    }
}

// === HELPER FUNCTIONS ===
void Client::connectToServer() const
{
    if (tcp_socket_->connect(server_ip_, server_tcp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Connecting: disconnected or some other error" << std::endl;
    }
    else {
        std::cout << "Connected to server via TCP" << std::endl;
    }

    // udp_socket_.setBlocking(false);
}

void Client::sendInputsToServer(const std::string& input)
{
    if (input.empty()) { return; }

    sf::Packet packet;
    packet << input;
    std::cout << "\nSending input: " << input << " to packet" << std::endl;
    if (udp_socket_.send(packet, server_ip_, server_udp_port_) != sf::Socket::Done) { std::cerr << "[ERROR] - Sending input to server" << std::endl; }
}

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
        packet << input;
        std::cout << "Adding input: " << input << " to packet" << std::endl;
    }
    if (udp_socket_.send(packet, server_ip_, server_udp_port_) != sf::Socket::Done) { std::cerr << "[ERROR] - Sending input to server" << std::endl; }
    std::cout << "Sent " << input_queue_.size() << " inputs to server" << std::endl;

    input_queue_.clear();
}

void Client::receiveSnapshotFromServer()
{
    sf::Packet packet;
    if (udp_socket_.receive(packet, server_ip_, server_udp_port_) != sf::Socket::Done) {
        std::cerr << "[ERROR] - Receiving snapshot from server" << std::endl;
    }
    Snapshot snapshot;
    packet >> snapshot;
    // std::cout << "Received snapshot from server" << std::endl;
    // std::cout << "Left paddle position: " << snapshot.left_paddle_position.x << ", " << snapshot.left_paddle_position.y << std::endl;
    // std::cout << "Right paddle position: " << snapshot.right_paddle_position.x << ", " << snapshot.right_paddle_position.y << std::endl;
    // std::cout << "Ball position: " << snapshot.ball_position.x << ", " << snapshot.ball_position.y << std::endl;
    // std::cout << "Ball speed: " << snapshot.ball_speed.x << ", " << snapshot.ball_speed.y << std::endl;
    // std::cout << "Left score: " << snapshot.left_score << std::endl;
    // std::cout << "Right score: " << snapshot.right_score << std::endl;

    game_->setLeftPaddlePosition(snapshot.left_paddle_position);
    game_->setRightPaddlePosition(snapshot.right_paddle_position);
    game_->setBallPosition(snapshot.ball_position);
    game_->setBallSpeed(snapshot.ball_speed);
    game_->setLeftScore(snapshot.left_score);
    game_->setRightScore(snapshot.right_score);
}
