#pragma once
#include "Pong.hpp"
#include <SFML/Network.hpp>
#include <list>
#include <memory>

class Server {
public:
    Server(const std::string& server_ip, const unsigned short server_tcp_port, const unsigned short server_udp_port)
        : server_ip_(server_ip), server_tcp_port_(server_tcp_port), server_udp_port_(server_udp_port)
    {}

    void run();

private:
    void listen();
    void handleNewConnections();

    void receiveInputFromClients();
    void updateGameBasedOnInput();
    void createAndSendSnapshotToClients();

    sf::IpAddress server_ip_;
    unsigned short server_tcp_port_;
    unsigned short server_udp_port_;

    sf::TcpListener listener_;
    sf::SocketSelector selector_;

    std::list<std::unique_ptr<sf::TcpSocket>> clients_;
    std::vector<sf::IpAddress> client_ips_;
    std::vector<unsigned short> client_ports_;
    sf::UdpSocket udp_socket_;

    std::unique_ptr<Pong> game_;

    std::vector<std::pair<size_t, std::string>> client_inputs_queue_;
};
