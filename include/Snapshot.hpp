//
// Created by Emil on 01/12/2024.
//

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <SFML/Network.hpp> // For sf::Packet

struct Snapshot {
    sf::Vector2f left_paddle_position;
    sf::Vector2f right_paddle_position;
    sf::Vector2f ball_position;
    sf::Vector2f ball_speed;
    int left_score;
    int right_score;
};

sf::Packet& operator<<(sf::Packet& packet, const Snapshot& snapshot);

sf::Packet& operator>>(sf::Packet& packet, Snapshot& snapshot);

#endif // SNAPSHOT_H
