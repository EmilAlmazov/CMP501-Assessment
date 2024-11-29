#pragma once

#include <SFML/Graphics.hpp>
#include <atomic>
#include <optional>

#include "../Client/Client.hpp"

class Pong
{
public:
    explicit Pong(std::atomic<bool> game_started, Client* client = nullptr);
	~Pong();

	void run();

    sf::Vector2f getPaddlePosition(size_t paddle_index) const;
    void setPaddlePosition(const sf::Vector2f& position, size_t paddle_index);

	sf::Vector2f getBallPosition() const;
	void setBallPosition(const sf::Vector2f& position);

private:
	void commonConstructor();

	void handleInput();
	void updateGame();
	void handlePaddleCollision(sf::RectangleShape& paddle);
	void handleNetworking();
	void render();

    sf::RenderWindow window_{sf::VideoMode(1920, 1080), "Pong Multiplayer", sf::Style::Default};
    sf::RectangleShape left_paddle_{sf::Vector2f{50.0f, 200.0f}};
    sf::RectangleShape right_paddle_{sf::Vector2f{50.0f, 200.0f}};
    sf::RectangleShape ball_{sf::Vector2f{50.0f, 50.0f}};
    sf::Vector2f ball_speed_{5.0f,5.0f};

    std::atomic<bool>& game_started_;
    int left_score_{0};
    int right_score_{0};
    sf::Text left_score_text_;
    sf::Text right_score_text_;
    sf::Text fps_;
    sf::Font font_;
    sf::Clock clock_;
    sf::Time time_;

	// Networking
	Client* client_;
};
