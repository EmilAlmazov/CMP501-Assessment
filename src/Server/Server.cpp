#include "Server.hpp"
#include <iostream>

// https://www.sfml-dev.org/documentation/2.6.1/classsf_1_1SocketSelector.php
void Server::run() {
    listen();
    selector_.add(listener_);

    // 1 - Wait for 2 clients to connect
    while (clients_.size() < 2) {
        // Wait on ANY sockets
        if (selector_.wait()) {
            // LISTENER socket is ready --> new client trying to connect --> accept it
            if (selector_.isReady(listener_)) {
                handleNewConnections();
                std::cout << "Number of clients: " << clients_.size() << std::endl;
            }
        }
    }

    // 2 - Both clients connected
    // 2.1 - send a start message to both clients
    sf::Packet packet;
    size_t paddleIndex = 0;

    for (auto it = clients_.begin(); it != clients_.end(); ++it, ++paddleIndex) {
        packet.clear();
        packet << paddleIndex << "start";
        if ((*it)->send(packet) != sf::Socket::Done) {
            std::cerr << "Error sending start message to client" << std::endl;
        }
    }

    // 2.2 Run the game
    Pong game{true};
    while (true) {
        game.run();

        // TODO: move to Pong::run()
        // float paddle_pos_x;
        // float paddle_pos_y;
        //
        // if (selector_.isReady(udp_socket_)) {
        //     sf::Packet packet;
        //     sf::IpAddress client_ip;
        //     unsigned short client_port;
        //
        //     // 2.2.1 - RECEIVE data from clients
        //     if (udp_socket_.receive(packet, client_ip, client_port) != sf::Socket::Done) {
        //         std::cerr << "Error receiving data from client" << std::endl;
        //         break;
        //     }
        //
        //     if (packet >> paddle_pos_x >> paddle_pos_y) {
        //         std::cout << "Received paddle position: " << paddle_pos_x << ", " << paddle_pos_y << std::endl;
        //     } else {
        //         std::cerr << "Error reading data from packet" << std::endl;
        //         break;
        //     }
        //
        //     // 2.2.2 - UPDATE GAME - paddles positions
        //     size_t client_index = client_ip == client_ips_[0] && client_port == client_ports_[0] ? 0 : 1;
        //     game.setPaddlePosition(sf::Vector2f{paddle_pos_x, paddle_pos_y}, client_index);
        //
        //     // 2.2.3 - SEND to other client - so +1 % 2
        //     packet << 0 << paddle_pos_x << paddle_pos_y;
        //     size_t other_client_index = (client_index + 1) % 2;
        //     if (udp_socket_.send(packet, client_ips_[other_client_index], client_ports_[other_client_index]) !=
        //         sf::Socket::Done) {
        //         std::cerr << "Error sending data to client" << std::endl;
        //     }
        // }
        //
        // // TODO: Send ball position to both clients regularly
        // send(game);
    }
}


// --------------------------------------------------------------------------------------
#pragma region HELPER FUNCTIONS
void Server::listen() {
    if (listener_.listen(tcp_port_) == sf::Socket::Done) {
        std::cout << "Listening on " << ip_ << ":" << tcp_port_ << std::endl;
    } else {
        std::cerr << "Error listening on " << ip_ << ":" << tcp_port_ << std::endl;
    }
}

void Server::handleNewConnections() {
    if (auto newSocket = std::make_unique<sf::TcpSocket>();
        listener_.accept(*newSocket) == sf::Socket::Done && clients_.size() < 2) {
        selector_.add(*newSocket);
        clients_.push_back(std::move(newSocket));

        // Receive client's port
        sf::Packet packet;
        if (clients_.back()->receive(packet) == sf::Socket::Done) {
            unsigned short udp_port;
            if (packet >> udp_port) {
                client_ips_.push_back(clients_.back()->getRemoteAddress());
                client_ports_.push_back(udp_port);
                std::cout << "Client connected from " << client_ips_.back() << ":" << client_ports_.back() << std::endl;
            } else {
                std::cerr << "[ERROR] - Reading client port failed" << std::endl;
                clients_.pop_back();
            }
        } else {
            std::cerr << "[ERROR] - Accepting client failed" << std::endl;
            clients_.pop_back();
        }
    } else {
        std::cerr << "Error accepting new client" << std::endl;
    }
}

// Receive data from clients
sf::Packet Server::receive() {
    if (selector_.isReady(udp_socket_)) {
        sf::Packet packet;
        sf::Socket::Status status = udp_socket_.receive(packet, ip_, tcp_port_);
        if (status == sf::Socket::Status::Done) {
            return packet;
        } else {
            std::cerr << "Error receiving data from client" << std::endl;
        }
    }
    return {};
}

// Send ball position to clients
void Server::send(const Pong &game) {
    for (size_t i = 0; i < clients_.size(); ++i) {
        sf::Packet packet;
        packet << 1 << game.getBallPosition().x << game.getBallPosition().y;
        if (udp_socket_.send(packet, client_ips_[i], client_ports_[i]) != sf::Socket::Done) {
            std::cerr << "Error sending data to client" << std::endl;
        }
    }
}


//void Server::handleExistingConnections()
//{
//    for (auto it = clients_.begin(); it != clients_.end(); )
//    {
//        sf::TcpSocket& client = **it;
//        if (selector_.isReady(client))
//        {
//            sf::Packet packet;
//            sf::Socket::Status status = client.receive(packet);
//            if (status == sf::Socket::Status::Done)
//            {
//                std::string message = extractMessageFromPacket(packet);
//                if (message == "quit") { removeDisconnectedClient(it); return; }
//
//                sendDataBack(client, packet);
//                ++it;
//            }
//            else
//            {
//                removeDisconnectedClient(it);
//            }
//        }
//        else
//        {
//            ++it;
//        }
//    }
//}
//
//std::string Server::extractMessageFromPacket(sf::Packet& packet)
//{
//    std::string message;
//    if (packet >> message)
//    {
//        std::cout << "Received message: " << message << std::endl;
//    }
//    else
//    {
//        std::cerr << "Error reading data from packet" << std::endl;
//    }
//    return message;
//}
//
//void Server::sendDataBack(sf::TcpSocket& socket, sf::Packet& packet)
//{
//    sf::Socket::Status status = socket.send(packet);
//    if (status != sf::Socket::Done)
//    {
//        std::cerr << "Error sending data back to client" << std::endl;
//    }
//}
//
//void Server::removeDisconnectedClient(std::list<std::unique_ptr<sf::TcpSocket>>::iterator& it)
//{
//    selector_.remove(**it);
//    it = clients_.erase(it);
//    std::cout << "Client disconnected" << std::endl;
//}
#pragma endregion
