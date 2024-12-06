#include "Server.hpp"
#include "Snapshot.hpp"
#include <iostream>

constexpr float PADDLE_SPEED = 20.0f; // TODO - probably move somewhere else?

// https://www.sfml-dev.org/documentation/2.6.1/classsf_1_1SocketSelector.php
void Server::run()
{
    // bind UDP socket
    if (udp_socket_.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cerr << "Error binding UDP socket" << std::endl;
        return;
    }
    server_udp_port_ = udp_socket_.getLocalPort();
    std::cout << "UDP socket bound to port: " << server_udp_port_ << std::endl;
    udp_socket_.setBlocking(false);

    listen();
    selector_.add(listener_);
    selector_.add(udp_socket_);

    // 1 - Wait for 2 clients to connect
    while (clients_.size() < 2) {
        // Wait on ANY sockets
        if (selector_.wait()) {
            // LISTENER socket is ready --> new client trying to connect -->
            // accept it
            if (selector_.isReady(listener_)) {
                handleNewConnections();
                std::cout << "Number of clients: " << clients_.size() << std::endl;
            }
        }
    }

    std::cout << "Both clients connected" << std::endl;

    // 2 - Send a start message to both clients
    sf::Packet packet;
    size_t client_index = 0;

    for (auto it = clients_.begin(); it != clients_.end(); ++it, ++client_index) {
        packet.clear();
        packet << "start" << client_index;
        if ((*it)->send(packet) != sf::Socket::Done) { std::cerr << "Error sending start message to client" << std::endl; }
        std::cout << "Start message sent to client: " << client_index << std::endl;
    }

    // 3 - Run the game
    game_ = std::make_unique<Pong>(true, "Server");
    sf::Clock clock;
    while (game_->windowIsOpen()) {
        for (auto event = sf::Event(); game_->getWindow().pollEvent(event);) {
            if (event.type == sf::Event::Closed) { game_->getWindow().close(); }
        }

        // run the game
        game_->handlePhysics();

        receiveInputFromClients();
        updateGameBasedOnInput();
        game_->render(); // DEBUG for now, TODO: Remove this line

        if (constexpr size_t snapshotsPerSecond = 20; clock.getElapsedTime().asMilliseconds() >= 1000 / snapshotsPerSecond) {
            createAndSendSnapshotToClients();
            clock.restart();
        }
    }
}

// --------------------------------------------------------------------------------------
#pragma region HELPER FUNCTIONS
void Server::listen()
{
    if (listener_.listen(server_tcp_port_) == sf::Socket::Done) {
        std::cout << "Listening on " << server_ip_ << ":" << server_tcp_port_ << std::endl;
    }
    else {
        std::cerr << "Error listening on " << server_ip_ << ":" << server_tcp_port_ << std::endl;
    }
}

void Server::handleNewConnections()
{
    if (auto newSocket = std::make_unique<sf::TcpSocket>(); listener_.accept(*newSocket) == sf::Socket::Done && clients_.size() < 2) {
        selector_.add(*newSocket);
        clients_.push_back(std::move(newSocket));

        // Send client index to client
        sf::Packet packet;
        packet << clients_.size() - 1;
        if (clients_.back()->send(packet) != sf::Socket::Done) {
            std::cerr << "Error sending client index to client" << std::endl;
            clients_.pop_back();
            return;
        }

        // Send udp port to client
        packet.clear();
        packet << server_udp_port_;
        if (clients_.back()->send(packet) != sf::Socket::Done) {
            std::cerr << "Error sending UDP port to client" << std::endl;
            clients_.pop_back();
            return;
        }

        // Receive client's port
        packet.clear();
        if (clients_.back()->receive(packet) == sf::Socket::Done) {
            if (unsigned short udp_port; packet >> udp_port) {
                client_ips_.push_back(clients_.back()->getRemoteAddress());
                client_ports_.push_back(udp_port);
                std::cout << "Client connected from " << client_ips_.back() << ":" << client_ports_.back() << std::endl;
            }
            else {
                std::cerr << "[ERROR] - Reading client port failed" << std::endl;
                clients_.pop_back();
            }
        }
        else {
            std::cerr << "[ERROR] - Accepting client failed" << std::endl;
            clients_.pop_back();
        }
    }
    else {
        std::cerr << "Error accepting new client" << std::endl;
    }
}

void Server::receiveInputFromClients()
{
    sf::IpAddress client_ip;
    unsigned short client_port;

    if (sf::Packet packet; udp_socket_.receive(packet, client_ip, client_port) == sf::Socket::Done) {
        // if (!packet.endOfPacket()) {
        if (std::string input; packet >> input) {
            const size_t client_index = (client_ip == client_ips_[0] && client_port == client_ports_[0]) ? 0 : 1;
            client_inputs_queue_.emplace_back(client_index, input);

            std::cout << "Received input: " << input << " from " << client_ip << ":" << client_port << std::endl;
        }
        else {
            std::cerr << "Error reading input from client" << std::endl;
        }
        // }
        // else {
        //     std::cout << "End of packet, no inputs" << std::endl;
        // }
    }
}

void Server::updateGameBasedOnInput()
{
    if (client_inputs_queue_.empty()) { return; }

    const auto& [client_index, input] = client_inputs_queue_.back();

    // for (auto it = client_inputs_queue_.begin(); it != client_inputs_queue_.end(); ++it) {
    // const auto& [client_index, input] = *it;
    std::cout << "Updating game based on input from client: " << client_index << " with input: " << input << std::endl;
    if (client_index == 0 && input == "W" && game_->getLeftPaddle().getPosition().y > 0) { game_->getLeftPaddle().move(0.0f, -PADDLE_SPEED); }
    else if (client_index == 0 && input == "S" && game_->getLeftPaddle().getPosition().y < 1080 - 200) {
        game_->getLeftPaddle().move(0.0f, PADDLE_SPEED);
    }
    else if (client_index == 1 && input == "Up" && game_->getRightPaddle().getPosition().y > 0) {
        game_->getRightPaddle().move(0.0f, -PADDLE_SPEED);
    }
    else if (client_index == 1 && input == "Down" && game_->getRightPaddle().getPosition().y < 1080 - 200) {
        game_->getRightPaddle().move(0.0f, PADDLE_SPEED);
    }
    else {
        std::cerr << "Invalid client index: " << client_index << " or input from client: " << input << std::endl;
    }
    // }

    client_inputs_queue_.pop_back();
    // client_inputs_queue_.clear();
}

void Server::createAndSendSnapshotToClients()
{
    Snapshot snapshot;
    snapshot.left_paddle_position = game_->getLeftPaddle().getPosition();
    snapshot.right_paddle_position = game_->getRightPaddle().getPosition();
    snapshot.ball_position = game_->getBall().getPosition();
    snapshot.ball_speed = game_->getBallSpeed();
    snapshot.left_score = game_->getLeftScore();
    snapshot.right_score = game_->getRightScore();

    sf::Packet packet;
    packet << snapshot;
    for (size_t i = 0; i < clients_.size(); ++i) {
        if (udp_socket_.send(packet, client_ips_[i], client_ports_[i]) != sf::Socket::Done) {
            std::cerr << "Error sending snapshot to client: " << i << std::endl;
        }
    }
}

#pragma endregion
