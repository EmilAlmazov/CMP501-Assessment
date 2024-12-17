//
// Created by Emil on 01/12/2024.
//

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "Pong.hpp"
#include <SFML/Network.hpp> // For sf::Packet
#include <memory>

class Snapshot {
public:
    explicit Snapshot(const std::unique_ptr<Pong>& game)
        : left_paddle_position(game->getLeftPaddle().getPosition()),
          right_paddle_position(game->getRightPaddle().getPosition()), ball_position(game->getBall().getPosition()),
          ball_speed(game->getBallSpeed()), left_score(game->getLeftScore()), right_score(game->getRightScore()) {};

    Snapshot() = default;

    friend sf::Packet& operator<<(sf::Packet& packet, const Snapshot& snapshot);

    friend sf::Packet& operator>>(sf::Packet& packet, Snapshot& snapshot);

    friend bool operator!=(const Snapshot& lhs, const Snapshot& rhs);

    // overload isEmpty() operator
    friend bool operator!(const Snapshot& snapshot);

    // overload std::cout operator
    friend std::ostream& operator<<(std::ostream& os, const Snapshot& snapshot);

    // GETTERs
    sf::Vector2f getLeftPaddlePosition() const { return left_paddle_position; }
    sf::Vector2f getRightPaddlePosition() const { return right_paddle_position; }
    sf::Vector2f getBallPosition() const { return ball_position; }
    sf::Vector2f getBallSpeed() const { return ball_speed; }
    int getLeftScore() const { return left_score; }
    int getRightScore() const { return right_score; }

    // SETTERs
    void setLeftPaddlePosition(const sf::Vector2f& position) { left_paddle_position = position; }
    void setRightPaddlePosition(const sf::Vector2f& position) { right_paddle_position = position; }
    void setBallPosition(const sf::Vector2f& position) { ball_position = position; }
    void setBallSpeed(const sf::Vector2f& speed) { ball_speed = speed; }
    void setLeftScore(const int score) { left_score = score; }
    void setRightScore(const int score) { right_score = score; }

private:
    sf::Vector2f left_paddle_position;
    sf::Vector2f right_paddle_position;
    sf::Vector2f ball_position;
    sf::Vector2f ball_speed;
    int left_score;
    int right_score;
    // sf::Time timestamp;
};

#endif // SNAPSHOT_H
