//
// Created by Emil on 01/12/2024.
//

#include "Snapshot.hpp"

sf::Packet& operator<<(sf::Packet& packet, const Snapshot& snapshot)
{
    return packet << snapshot.left_paddle_position.x << snapshot.left_paddle_position.y << snapshot.right_paddle_position.x
                  << snapshot.right_paddle_position.y << snapshot.ball_position.x << snapshot.ball_position.y << snapshot.ball_speed.x
                  << snapshot.ball_speed.y << snapshot.left_score << snapshot.right_score;
}

sf::Packet& operator>>(sf::Packet& packet, Snapshot& snapshot)
{
    return packet >> snapshot.left_paddle_position.x >> snapshot.left_paddle_position.y >> snapshot.right_paddle_position.x
           >> snapshot.right_paddle_position.y >> snapshot.ball_position.x >> snapshot.ball_position.y >> snapshot.ball_speed.x
           >> snapshot.ball_speed.y >> snapshot.left_score >> snapshot.right_score;
}