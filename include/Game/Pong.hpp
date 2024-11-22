#pragma once

#include <SFML/Graphics.hpp>
#include <atomic>

class Pong
{
public:
    Pong(std::atomic<bool> game_started);
	~Pong();

	void run();

    sf::Vector2f getPaddlePosition(size_t paddle_index) const;
    void setPaddlePosition(const sf::Vector2f& position, size_t paddle_index);

private:
	void handlePaddleCollision(sf::RectangleShape& paddle);

    sf::RenderWindow window_;
    sf::RectangleShape left_paddle_;
    sf::RectangleShape right_paddle_;
    sf::RectangleShape ball_;
    sf::Vector2f ball_speed_;

    std::atomic<bool>& game_started_;
    int left_score_;
    int right_score_;
    sf::Text left_score_text_;
    sf::Text right_score_text_;
    sf::Text fps_;
    sf::Font font_;
    sf::Clock clock_;
    sf::Time time_;
};