//
// Created by Emil on 01/12/2024.
//

#include "Snapshot.hpp"

sf::Packet &operator<<(sf::Packet &packet, const Snapshot &snapshot) {
    return packet << snapshot.left_paddle_position.x << snapshot.left_paddle_position.y << snapshot.
           right_paddle_position.x
           << snapshot.right_paddle_position.y << snapshot.ball_position.x << snapshot.ball_position.y << snapshot.
           ball_speed.x
           << snapshot.ball_speed.y << snapshot.left_score << snapshot.right_score;
}

sf::Packet &operator>>(sf::Packet &packet, Snapshot &snapshot) {
    return packet >> snapshot.left_paddle_position.x >> snapshot.left_paddle_position.y >> snapshot.
           right_paddle_position.x
           >> snapshot.right_paddle_position.y >> snapshot.ball_position.x >> snapshot.ball_position.y >> snapshot.
           ball_speed.x >> snapshot.ball_speed.y
           >> snapshot.left_score >> snapshot.right_score;
}

bool operator!=(const Snapshot &lhs, const Snapshot &rhs) {
    return lhs.left_paddle_position != rhs.left_paddle_position || lhs.right_paddle_position != rhs.
           right_paddle_position
           || lhs.ball_position != rhs.ball_position || lhs.ball_speed != rhs.ball_speed || lhs.left_score != rhs.
           left_score
           || lhs.right_score != rhs.right_score;
}

bool operator!(const Snapshot &snapshot) {
    return snapshot.left_paddle_position == sf::Vector2f() && snapshot.right_paddle_position == sf::Vector2f()
           && snapshot.ball_position == sf::Vector2f() && snapshot.ball_speed == sf::Vector2f() && snapshot.left_score
           == 0
           && snapshot.right_score == 0;
}

std::ostream &operator<<(std::ostream &os, const Snapshot &snapshot) {
    os << "Left paddle position: " << snapshot.left_paddle_position.x << " " << snapshot.left_paddle_position.y <<
            std::endl;
    os << "Right paddle position: " << snapshot.right_paddle_position.x << " " << snapshot.right_paddle_position.y <<
            std::endl;
    os << "Ball position: " << snapshot.ball_position.x << " " << snapshot.ball_position.y << std::endl;
    os << "Ball speed: " << snapshot.ball_speed.x << " " << snapshot.ball_speed.y << std::endl;
    os << "Left score: " << snapshot.left_score << std::endl;
    os << "Right score: " << snapshot.right_score << std::endl;

    return os;
}
